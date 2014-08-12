#include "Common.h"
#include "Geometry.h"
#include "GraphicFunctions.h"

LPDIRECTDRAWCLIPPER   lpddclipper  = NULL;   // dd clipper
DDBLTFX               ddbltfx;               // used to fill
HRESULT               ddrval;                // result back from dd calls
DWORD                 start_clock_count = 0; // used for timing


char buffer[80];                     // general printing buffer
Canvas *lp_canvas = NULL;

bool closed = false;

Object3d cube;
float cx[8] = {-1,-1,1,1,-1,1,1,-1};
float cy[8] = {-1,-1,-1,-1,1,1,1,1};
float cz[8] = {0,2,0,2,0,0,2,2};
int index[36] = {2,3,1,2,1,0,4,5,2,4,2,0,6,3,2,6,2,5,6,7,1,6,1,3,6,5,4,6,4,7,1,7,4,1,4,0};
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
LRESULT CALLBACK WindowProc(HWND hwnd, 
						    UINT msg, 
                            WPARAM wparam, 
                            LPARAM lparam)
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


int Game_Main(void *parms = NULL, int num_parms = 0)
{

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
		lp_canvas->lp_camera->pos.x +=10; 
	}
	if(moveleft){
		lp_canvas->lp_camera->pos.x -=10; 
	}
	if(movefront){
		lp_canvas->lp_camera->pos.z +=10;
	}
	if(moveback){
		lp_canvas->lp_camera->pos.z -=10;
	}
	if(moveup){
		lp_canvas->lp_camera->pos.y +=10;
	}
	if(movedown){
		lp_canvas->lp_camera->pos.y -=10;
	}
	lp_canvas->lock();
	lp_canvas->clear();
	static float a = 0;
	a+=1;
	cube.rotationY(a);
	lp_canvas->render();
	lp_canvas->unlock();
	lp_canvas->flip();
	Sleep(30);
	return(1);

}

void createObject(){
	Vertex3d *v = NULL;
	Poly *p = NULL;
	cube.init(8,12,1);
	cube.max_radius = 100;
	cube.world_pos.z = 500;
	for (int i = 0;i<cube.init_num_vertexs;i++)
	{
		v = new Vertex3d();
		v->x = cx[i]*100;
		v->y = cy[i]*100;
		v->z = cz[i]*100;
		v->w = 1;
		cube.addVertex(v);
		delete v;
	}
	for (int j = 0;j<cube.init_num_polys;j++)
	{
		Poly* temp = new Poly;
		temp->v_index_list[0] = index[j*3];
		temp->v_index_list[1] = index[j*3+1];
		temp->v_index_list[2] = index[j*3+2];
		temp->lp_vertex_object = cube.lp_vertex_trans;
		temp->color = 0xffff0000;
		cube.lp_polys[cube.num_polys] = *temp;
		cube.num_polys++;
		delete temp;
		temp = NULL;
		//cube.addPoly(index[j*3],index[j*3+1],index[j*3+2]);
	}
}
int WINAPI WinMain(	HINSTANCE hinstance,
					HINSTANCE hprevinstance,
					LPSTR lpcmdline,
					int ncmdshow)
{
	lp_canvas = new Canvas();
	lp_canvas->init(hinstance,WindowProc,SCREEN_WIDTH,SCREEN_HEIGHT,SCREEN_BPP,1);
	createObject();
	Camera *camera = new Camera();
	Vector3d dir;
	dir.init(0,0,0,1);
	Point3d pos;
	pos.init(0,140,-100,1);
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

