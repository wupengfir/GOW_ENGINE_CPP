// DEMO6_3.CPP basic full-screen pixel plotting DirectDraw demo

// INCLUDES ///////////////////////////////////////////////


#include "Common.h"
// GLOBALS ////////////////////////////////////////////////


// directdraw stuff

LPDIRECTDRAW7         lpdd         = NULL;   // dd object
LPDIRECTDRAWSURFACE7  lpddsprimary = NULL;   // dd primary surface
LPDIRECTDRAWSURFACE7  lpddsback    = NULL;   // dd back surface
LPDIRECTDRAWPALETTE   lpddpal      = NULL;   // a pointer to the created dd palette
LPDIRECTDRAWCLIPPER   lpddclipper  = NULL;   // dd clipper
PALETTEENTRY          palette[256];          // color palette
PALETTEENTRY          save_palette[256];     // used to save palettes
DDSURFACEDESC2        ddsd;                  // a direct draw surface description struct
DDBLTFX               ddbltfx;               // used to fill
DDSCAPS2              ddscaps;               // a direct draw surface capabilities struct
HRESULT               ddrval;                // result back from dd calls
DWORD                 start_clock_count = 0; // used for timing

// these defined the general clipping rectangle
int min_clip_x = 0,                          // clipping rectangle 
    max_clip_x = 0,
    min_clip_y = 0,
    max_clip_y = 0;

// these are overwritten globally by DD_Init()
int screen_width  = SCREEN_WIDTH,            // width of screen
    screen_height = SCREEN_HEIGHT,           // height of screen
    screen_bpp    = SCREEN_BPP;              // bits per pixel


char buffer[80];                     // general printing buffer
Canvas *lp_canvas;

bool closed = false;
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
	Matrix m(23);
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
	m44.mulVector3d(&v,&v1);

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
	static int a = 0;
	a++;
	for (int index=0; index < 500; index++)
	{		
		a = a==800?0:a;
		unsigned int color = _RGB32BIT(rand()%256,rand()%256,rand()%256,rand()%256);
		int x =  rand()%SCREEN_WIDTH*2 - SCREEN_WIDTH;
		int y =  rand()%SCREEN_HEIGHT*2 - SCREEN_HEIGHT;//a;
		Point2d p1;
		Point2d p2;
		p1.M[0] = x;
		p1.M[1] = y;
		p2.M[0] = rand()%SCREEN_WIDTH;
		p2.M[1] = rand()%SCREEN_HEIGHT;
		//lp_canvas->plotPixel(x,y,color);     
		//drawLine(x,y,rand()%SCREEN_WIDTH,rand()%SCREEN_HEIGHT,color);
		drawLine(p1.x,p1.y,p2.x,p1.y,color);

		//drawLine(396,-50,694,-50,color);
	}
	lp_canvas->unlock();
	lp_canvas->flip();
	Sleep(30);
	return(1);

}

int WINAPI WinMain(	HINSTANCE hinstance,
					HINSTANCE hprevinstance,
					LPSTR lpcmdline,
					int ncmdshow)
{
	lp_canvas = new Canvas();
MSG		   msg;		 // generic message
lp_canvas->init(hinstance,WindowProc,SCREEN_WIDTH,SCREEN_HEIGHT,SCREEN_BPP,1);
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

