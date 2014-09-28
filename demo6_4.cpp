#include "Common.h"
#include "Geometry.h"
#include "GraphicFunctions.h"
#include "Loader.h"
#include "Test.h"

LPDIRECTDRAWCLIPPER   lpddclipper  = NULL;   // dd clipper
DDBLTFX               ddbltfx;               // used to fill
HRESULT               ddrval;                // result back from dd calls


Canvas *lp_canvas = NULL;
bool closed = false;






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
//bool moveback = false;
//bool moveright = false;
//bool moveleft = false;
//bool movefront = false;
//bool moveup = false;
//bool movedown = false;
//bool turnright = false;
//bool turnleft = false;
//void checkKey(){
//	if (KEYUP(83)) {
//		moveback=false;
//	}  if (KEYUP(69)) {
//		moveright=false;
//	}  if (KEYUP(81)) {
//		moveleft=false;
//	}  if (KEYUP(87)) {
//		movefront=false;
//	} if (KEYUP(32)) {
//		moveup=false;
//	} if (KEYUP(82)) {
//		movedown=false;
//	} if (KEYUP(65)) {
//		turnleft=false;
//	} if (KEYUP(68)) {
//		turnright=false;
//	}
//
//	if (KEYDOWN(83)) {
//		moveback=true;
//	}  if (KEYDOWN(69)) {
//		moveright=true;
//	}  if (KEYDOWN(81)) {
//		moveleft=true;
//	}  if (KEYDOWN(87)) {
//		movefront=true;
//	} if (KEYDOWN(32)) {
//		moveup=true;
//	} if (KEYDOWN(82)) {
//		movedown=true;
//	} if (KEYDOWN(65)) {
//		turnleft=true;
//	} if (KEYDOWN(68)) {
//		turnright=true;
//	}
//}

void checkPosition(){

}

float ry = 0;
UINT start_clock = 0;
int fps = 0;
int fps_show = 0;
int second = 0;
int Game_Main(void *parms = NULL, int num_parms = 0)
{
	int current_clock = timeGetTime();
	if ((current_clock - start_clock) < 30) 
	{
		return 1;
	}
	start_clock = current_clock;
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
	main_loop_1();

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




int WINAPI WinMain(	HINSTANCE hinstance,HINSTANCE hprevinstance,LPSTR lpcmdline,int ncmdshow)
{
	lp_canvas = new Canvas();
	lp_canvas->init(hinstance,WindowProc,SCREEN_WIDTH,SCREEN_HEIGHT,SCREEN_BPP,1);

	Camera *camera = new Camera();
	Vector3d dir;
	dir.init(0,0,0,1);
	Point3d pos;
	pos.init(0,150,-100,1);
	Point3d target;
	target.init(0,0,0,0);
	camera->init(Camera::CAMERA_TYPE_EULER,pos,dir,target,100,2000,90,SCREEN_WIDTH,SCREEN_HEIGHT);
	lp_canvas->setCamera(camera);

	init_2();

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

