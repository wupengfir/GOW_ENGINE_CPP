// DEMO6_3.CPP basic full-screen pixel plotting DirectDraw demo

// INCLUDES ///////////////////////////////////////////////


#include "InitD3D.h"
// GLOBALS ////////////////////////////////////////////////
HWND      main_window_handle = NULL; // globally track main window
HINSTANCE hinstance_app      = NULL; // globally track hinstance

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
    max_clip_x = SCREEN_WIDTH-1,
    min_clip_y = 0,
    max_clip_y = SCREEN_HEIGHT-1;

// these are overwritten globally by DD_Init()
int screen_width  = SCREEN_WIDTH,            // width of screen
    screen_height = SCREEN_HEIGHT,           // height of screen
    screen_bpp    = SCREEN_BPP;              // bits per pixel


char buffer[80];                     // general printing buffer
InitD3D init;
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

///////////////////////////////////////////////////////////

int Game_Main(void *parms = NULL, int num_parms = 0)
{
if (KEYDOWN(VK_ESCAPE))
   SendMessage(main_window_handle,WM_CLOSE,0,0);
memset(&ddsd,0,sizeof(ddsd)); 
ddsd.dwSize = sizeof(ddsd);

if (FAILED(lpddsback->Lock(NULL, &ddsd,
                   DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT,
                   NULL)))
   {
   // error
   return(0);
   } // end if
int mempitch        = (int)ddsd.lPitch;
UCHAR *video_buffer = (UCHAR *)ddsd.lpSurface;
// plot 1000 random pixels with random colors on the
// primary surface, they will be instantly visible
static int a = 0;
a++;
for (int index=0; index < 500; index++)
    {
		
		a = a==300?0:a;
    // select random position and color for 640x480x8
    unsigned int color = _RGB32BIT(255,rand()%256,rand()%256,rand()%256);
    int x =  index%SCREEN_WIDTH;
    int y =  a;
    // plot the pixel
	if(mempitch == 0){
		((unsigned int*)video_buffer)[x+y*500/4] = color; 
	}else{
		((unsigned int*)video_buffer)[x+y*mempitch/4] = color;  
	}

          
    } // end for index

// now unlock the primary surface
if (FAILED(lpddsback->Unlock(NULL)))
   return(0);
lpddsprimary->Flip(NULL,DDFLIP_WAIT);
// sleep a bit
Sleep(30);

// return success or failure or your own return code here
return(1);

} // end Game_Main


int Game_Shutdown(void *parms = NULL, int num_parms = 0)
{
// this is called after the game is exited and the main event
// loop while is exited, do all you cleanup and shutdown here

// first the palette
if (lpddpal)
   {
   lpddpal->Release();
   lpddpal = NULL;
   } // end if

// now the primary surface
if (lpddsprimary)
   {
   lpddsprimary->Release();
   lpddsprimary = NULL;
   } // end if

// now blow away the IDirectDraw4 interface
if (lpdd)
   {
   lpdd->Release();
   lpdd = NULL;
   } // end if

// return success or failure or your own return code here
return(1);

} // end Game_Shutdown

// WINMAIN ////////////////////////////////////////////////

int WINAPI WinMain(	HINSTANCE hinstance,
					HINSTANCE hprevinstance,
					LPSTR lpcmdline,
					int ncmdshow)
{
HWND	   hwnd;	 // generic window handle
MSG		   msg;		 // generic message
HDC        hdc;      // graphics device context
// save hinstance in global
hinstance_app = hinstance;

//if (!(hwnd = init.createWindow(hinstance,WindowProc,SCREEN_WIDTH,SCREEN_HEIGHT)))	// extra creation parms
//return(0);
//main_window_handle = hwnd;
//init.initDDraw(&lpdd,main_window_handle,DDSCL_FULLSCREEN | DDSCL_ALLOWMODEX | 
//	DDSCL_EXCLUSIVE | DDSCL_ALLOWREBOOT,&ddsd,&lpddsprimary,&lpddsback);


init.lp_canva = new Canvas();
init.lp_canva->init(hinstance,WindowProc,SCREEN_WIDTH,SCREEN_HEIGHT,SCREEN_BPP,0);
lpdd = init.lp_canva->lp_directdraw;
lpddsprimary = init.lp_canva->lp_primary_surface;
lpddsback = init.lp_canva->lp_back_surface;

main_window_handle = init.lp_canva->main_handler;
while(TRUE)
	{
    // test if there is a message in queue, if so get it
	if (PeekMessage(&msg,NULL,0,0,PM_REMOVE))
	   { 
	   // test if this is a quit
       if (msg.message == WM_QUIT)
           break;
	
	   // translate any accelerator keys
	   TranslateMessage(&msg);

	   // send the message to the window proc
	   DispatchMessage(&msg);
	   } // end if
    
       // main game processing goes here
       Game_Main();
       
	} // end while

// closedown game here
Game_Shutdown();

// return to Windows like this
return(msg.wParam);

} // end WinMain

///////////////////////////////////////////////////////////

