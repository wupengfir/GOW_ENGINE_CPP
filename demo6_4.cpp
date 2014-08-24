#include "Common.h"
#include "Geometry.h"
#include "GraphicFunctions.h"
#include "Loader.h"
LPDIRECTDRAWCLIPPER   lpddclipper  = NULL;   // dd clipper
DDBLTFX               ddbltfx;               // used to fill
HRESULT               ddrval;                // result back from dd calls


Canvas *lp_canvas = NULL;
bool closed = false;

Object3d cube;
float cx[8] = {-1,-1,1,1,-1,1,1,-1};
float cy[8] = {-1,-1,-1,-1,1,1,1,1};
float cz[8] = {0,2,0,2,0,0,2,2};
int index[36] = {4,2,0,6,3,2,6,2,5,6,7,1,6,1,3,6,5,4,6,4,7,1,7,4,1,4,0};
bool moveback = false;
bool moveright = false;
bool moveleft = false;
bool movefront = false;
bool moveup = false;
bool movedown = false;
bool turnright = false;
bool turnleft = false;
//
//float cx[3] = {-1,-1,1};
//float cy[3] = {-1,-1,-1};
//float cz[3] = {0,2,0};
//int index[6] = {2,3,1,2,1,0};

// FUNCTIONS //////////////////////////////////////////////
char buffer[128];
LRESULT CALLBACK WindowProc(HWND hwnd,UINT msg, WPARAM wparam, LPARAM lparam)
{
// this is the main message handler of the system
PAINTSTRUCT		ps;		// used in WM_PAINT
HDC				hdc;	// handle to a device context
char buffer[80];        // used to print strings


// what is the message 
switch(msg)
	{	
	case WM_CREATE: 
        {
		// do initialization stuff here
        // return success
		return(0);
		} break;
   
	case WM_PAINT: 
		{
		// simply validate the window 
   	    hdc = BeginPaint(hwnd,&ps);	 
        
        // end painting
        EndPaint(hwnd,&ps);

        // return success
		return(0);
   		} break;

	case WM_DESTROY: 
		{

		// kill the application, this sends a WM_QUIT message 
		PostQuitMessage(0);

        // return success
		return(0);
		} break;

	default:break;

    } // end switch

// process any messages that we didn't take care of 
return (DefWindowProc(hwnd, msg, wparam, lparam));

} 

void checkKey(){
	if (KEYUP(83)) {
		moveback=false;
	}  if (KEYUP(69)) {
		moveright=false;
	}  if (KEYUP(81)) {
		moveleft=false;
	}  if (KEYUP(87)) {
		movefront=false;
	} if (KEYUP(32)) {
		moveup=false;
	} if (KEYUP(82)) {
		movedown=false;
	} if (KEYUP(65)) {
		turnleft=false;
	} if (KEYUP(68)) {
		turnright=false;
	}

	if (KEYDOWN(83)) {
		moveback=true;
	}  if (KEYDOWN(69)) {
		moveright=true;
	}  if (KEYDOWN(81)) {
		moveleft=true;
	}  if (KEYDOWN(87)) {
		movefront=true;
	} if (KEYDOWN(32)) {
		moveup=true;
	} if (KEYDOWN(82)) {
		movedown=true;
	} if (KEYDOWN(65)) {
		turnleft=true;
	} if (KEYDOWN(68)) {
		turnright=true;
	}
}

float ry = 0;
UINT start_clock = 0;
BitmapData image;
int fps = 0;
int fps_show = 0;
int second = 0;
int Game_Main(void *parms = NULL, int num_parms = 0)
{
	int current_clock = timeGetTime();
	if ((current_clock - start_clock) < 33)
	{
		return 1;
	}
	start_clock = current_clock;
	Vector3d direction;
	Vector3d result;
	direction.init(0,0,0,1);
	result.init(0,0,0,1);
	if (KEYDOWN(VK_ESCAPE)){
		SendMessage(lp_canvas->main_handler,WM_CLOSE,0,0);
		closed = true;
	}
	if (KEYDOWN(87))
	{
	}
	if (closed)
	{
		return 1;
	}
	checkKey();
	if(moveright){
		direction.x +=10; 
	}
	if(moveleft){
		direction.x -=10; 
	}
	if(movefront){
		direction.z +=10;
	}
	if(moveback){
		direction.z -=10;
	}
	if(moveup){
		direction.y +=10;
	}
	if(movedown){
		direction.y -=10;
	}
	if(turnright){
		lp_canvas->lp_camera->dir.y += 0.1;
	}
	if(turnleft){
		lp_canvas->lp_camera->dir.y -= 0.1;
	}
	if (moveback||movedown||movefront||moveleft||moveright||moveup||turnleft||turnright)
	{

		Matrix mx_inv(44);
		Matrix my_inv(44);
		Matrix mz_inv(44);
		float t_x = lp_canvas->lp_camera->dir.x;
		float t_y = lp_canvas->lp_camera->dir.y;
		float t_z = lp_canvas->lp_camera->dir.z;
		float cos_t = cos(t_x);
		float sin_t = sin(t_x);
		float x_data[16] = {1,0,0,0,
			0,cos_t,sin_t,0,
			0,-sin_t,cos_t,0,
			0,0,0,1};
		cos_t = cos(t_y);
		sin_t = sin(t_y);
		float y_data[16] = {cos_t,0,-sin_t,0,
			0,1,0,0,
			sin_t,0,cos_t,0,
			0,0,0,1};
		cos_t = cos(t_z);
		sin_t = sin(t_z);
		float z_data[16] = {cos_t,sin_t,0,0,
			-sin_t,cos_t,0,0,
			0,0,1,0,
			0,0,0,1};
		mx_inv.init(x_data);
		my_inv.init(y_data);
		mz_inv.init(z_data);

		Matrix storage_yz(44);
		Matrix storage_yzx(44);
		my_inv.multiply(&mz_inv,&storage_yz);
		storage_yz.multiply(&mx_inv,&storage_yzx);

		storage_yzx.mulVector3d(&direction,&result);
		lp_canvas->lp_camera->pos.x += result.x;
		lp_canvas->lp_camera->pos.y += result.y;
		lp_canvas->lp_camera->pos.z += result.z;
	}
	static bool lightoff = false;
	static bool lightable = true;
	if (KEYDOWN(int('T')))
	{
		if (lightable)
		{
			if (lightoff)
			{
				lp_canvas->LightList[1]->state = Light::LIGHTV1_STATE_ON;
				lightoff = false;
			}else{
				lp_canvas->LightList[1]->state = Light::LIGHTV1_STATE_OFF;
				lightoff = true;
			}
		}
		lightable = false;		
	}
	if (KEYUP(int('T')))
	{
		lightable = true;
	}
	

	lp_canvas->lock();
	lp_canvas->clear();
	ry+=1;
	cube.rotationY(ry);

	////////////////////////////////
	//Color *test_color = new Color[3];
	//test_color[0].init(0xffff2233);
	//test_color[1].init(0xff22ff33);
	//test_color[2].init(0xff2233ff);
	//Draw_Gouraud_Triangle(100,100,100,300,200,300,test_color[0],test_color[1],test_color[2],lp_canvas->lp_backbuffer,lp_canvas->lpitch);
	//Draw_Gouraud_Triangle(100,450,100,300,200,300,test_color[0],test_color[1],test_color[2],lp_canvas->lp_backbuffer,lp_canvas->lpitch);
	////////////////////////////////

	//Vertex3d v1;
	//Vertex3d v2;
	//Vertex3d v3;
	//v1.x = 100;v1.y = 100;v2.x = 100;v2.y = 300;v3.x = 200;v3.y = 450;
	//v1.tx = 0;v1.ty = 0;v2.tx = 0;v2.ty = 1;v3.tx = 1;v3.ty = 1;
	//drawTextureTriangle(&v1,&v2,&v3,&image,lp_canvas->lp_backbuffer,lp_canvas->lpitch);
	//v1.x = 100;v1.y = 100;v2.x = 200;v2.y = 250;v3.x = 200;v3.y = 450;
	//v1.tx = 0;v1.ty = 0;v2.tx = 1;v2.ty = 0;v3.tx = 1;v3.ty = 1;
	//drawTextureTriangle(&v1,&v2,&v3,&image,lp_canvas->lp_backbuffer,lp_canvas->lpitch);
	//lp_canvas->showImage(&image,300,100);


	lp_canvas->render();


	lp_canvas->unlock();

	//¼ÆËãÖ¡Êý
	if ((timeGetTime() - second)<1000)
	{
		fps++;
	}else{		
		fps_show = fps;
		fps = 0;
		second = timeGetTime();
	}
	sprintf_s(buffer,"fps = %d",fps_show);
	Draw_Text_GDI(buffer,0,0,RGB(255,255,255));
	lp_canvas->flip();
	return(1);

}

void createLight(){
	Light *light = NULL;

	light = new Light();
	light->init(Light::LIGHTV1_STATE_ON,Light::LIGHTV1_ATTR_AMBIENT,(new Color())->init(_RGB32BIT(0xff,60,60,60)),0,0,0,0,0,NULL,NULL,0,0,0);
	lp_canvas->addLight(light);

	light = new Light();
	Vector3d* v = new Vector3d();
	v->init(-1,2,-0.5,0);
	normalizeVector3d(v);
	light->init(Light::LIGHTV1_STATE_ON,Light::LIGHTV1_ATTR_INFINITE,0,(new Color())->init(_RGB32BIT(0xff,100,200,100)),0,0,0,0,NULL,v,0,0,0);
	lp_canvas->addLight(light);

	light = new Light();
	Point3d* p = new Point3d();
	p->init(0,1000,0);
	light->init(Light::LIGHTV1_STATE_ON,Light::LIGHTV1_ATTR_POINT,0,0,(new Color())->init(_RGB32BIT(0xff,100,100,200)),0.001,0,0,p,NULL,0,0,0);
	lp_canvas->addLight(light);
}
void createObject(){
	Vertex3d *v = NULL;
	Poly *p = NULL;
	cube.init(8,1,1);
	cube.max_radius = 100;
	cube.world_pos.z = 500;
	for (int i = 0;i<cube.num_vertices;i++)
	{
		v = new Vertex3d();
		v->x = cx[i]*100;
		v->y = cy[i]*100;
		v->z = cz[i]*100;
		v->w = 1;
		cube.lp_vertex_local[i] = *v;
		//lp_vertex_trans[num_vertices] = *v;
		//cube.addVertex(v);
		delete v;
	}
	for (int j = 0;j<cube.num_polys;j++)
	{
		Poly* temp = new Poly;
		temp->v_index_list[0] = index[j*3];
		temp->v_index_list[1] = index[j*3+1];
		temp->v_index_list[2] = index[j*3+2];
		temp->lp_vertex_object = cube.lp_vertex_trans;
		temp->attr = POLY4D_ATTR_SHADE_MODE_GOURAUD;
		temp->color.argb = 0xffffffff;
		cube.lp_polys[j] = *temp;
		delete temp;
		temp = NULL;
		//cube.addPoly(index[j*3],index[j*3+1],index[j*3+2]);
	}
}
void createImage(){
	image.width = 128;
	image.height = 128;
	image.power_of_two = 7;
	image.data = new Color[128*128];
	for (int i = 0;i<128;i++)
	{
		for (int j = 0;j<128;j++)
		{
			image.data[(i<<7) + j].argb = _RGB32BIT(0xff,i*2,0,0); 
		}
	}
}
int WINAPI WinMain(	HINSTANCE hinstance,HINSTANCE hprevinstance,LPSTR lpcmdline,int ncmdshow)
{
	lp_canvas = new Canvas();
	lp_canvas->init(hinstance,WindowProc,SCREEN_WIDTH,SCREEN_HEIGHT,SCREEN_BPP,1);
	//createObject();
	createLight();
	Point3d position;
	position.init(0,0,500);
	Vector3d scale;

	scale.init(100,100,100);
	Matrix rotation(44);
	float m[16] = {1,0,0,0,0,0,-1,0,0,1,0,0,0,0,0,1};
	rotation.init(m);
	loadObject_ASC("car01.asc",&cube,&position,&scale,&rotation,POLY4D_ATTR_SHADE_MODE_TEXTURE);
	//loadObject_COB("cube_flat_textured_01.cob",&cube,&position,&scale,0,POLY4D_ATTR_SHADE_MODE_TEXTURE,true);
	loadBitmapImage(&image,"img/123.bmp");cube.texture = &image;

	Camera *camera = new Camera();
	Vector3d dir;
	dir.init(0,0,0,1);
	Point3d pos;
	pos.init(0,150,-100,1);
	Point3d target;
	target.init(0,0,0,0);

	camera->init(Camera::CAMERA_TYPE_EULER,pos,dir,target,100,1000,90,SCREEN_WIDTH,SCREEN_HEIGHT);
	lp_canvas->setCamera(camera);
	lp_canvas->addObject(&cube);

	MSG		   msg;		 // generic message
	while(TRUE)
		{		
		if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
		   { 
		   if (msg.message == WM_QUIT)
			   break;
		   TranslateMessage(&msg);
		   DispatchMessage(&msg);
		   } 
		   Game_Main();      
		}
	delete lp_canvas;
	return(msg.wParam);

}

