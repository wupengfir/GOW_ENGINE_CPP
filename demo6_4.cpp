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

} // end WinProc

int Game_Main(void *parms = NULL, int num_parms = 0)
{
	/*Matrix m(23);
	float k[6] = {3,6,7,4,5,8};
	m.init(k);
	Matrix m1(22);
	float b[4] = {2,6,4,5};
	m1.init(b);
	Matrix s(23);
	m1.multiply(&m,&s);

	Vector3d v = {2.2,3,6,0};
	Vector3d v1;
	vector3dInit(&v1);
	Matrix m44(44);
	float cc[16] = {1,2,3,4,5,6,7,8,9,1,2,3,4,5,6,7};
	m44.init(cc);
	m44.mulVector3d(&v,&v1);*/

	if (KEYDOWN(VK_ESCAPE)){
		SendMessage(lp_canvas->main_handler,WM_CLOSE,0,0);
		closed = true;
	}		
	if (closed)
	{
		return 1;
	}
	lp_canvas->lock();
	lp_canvas->clear();
	//static int a = 0;
	//a++;
	//for (int index=0; index < 500; index++)
	//{		
	//	a = a==800?0:a;
	//	unsigned int color = _RGB32BIT(rand()%256,rand()%256,rand()%256,rand()%256);
	//	int x =  rand()%SCREEN_WIDTH*2 - SCREEN_WIDTH;
	//	int y =  rand()%SCREEN_HEIGHT*2 - SCREEN_HEIGHT;//a;
	//	Point2d p1;
	//	Point2d p2;
	//	p1.M[0] = x;
	//	p1.M[1] = y;
	//	p2.M[0] = rand()%SCREEN_WIDTH;
	//	p2.M[1] = rand()%SCREEN_HEIGHT;
	//	drawLine(p1.x,p1.y,p2.x,p1.y,color);
	//}
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
	cube.world_pos.z = 200;
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
		cube.addPoly(index[j*3],index[j*3+1],index[j*3+2]);
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

