#include "Canvas.h"


Canvas::Canvas(void)
{
}

void Canvas::reset(){
	lp_directdraw = NULL;
	lp_primary_surface = NULL;
	lp_back_surface = NULL;
	lp_bitmapdata = NULL;
	main_handler = 0;
	main_instance = 0;
}

void Canvas::flip(){

}

void Canvas::clear(){

}

void Canvas::init(HINSTANCE hinstance,WNDPROC callback,int width,int height,int bpp,int windowed){
	reset();
	main_instance = hinstance;
	is_window = windowed;
	this->bpp = bpp;
	this->width = width;
	this->height = height;
	WNDCLASSEX winclass; // this will hold the class we create
	// first fill in the window class stucture
	winclass.cbSize         = sizeof(WNDCLASSEX);
	winclass.style			= CS_DBLCLKS | CS_OWNDC | 
		CS_HREDRAW | CS_VREDRAW;
	winclass.lpfnWndProc	= callback;
	winclass.cbClsExtra		= 0;
	winclass.cbWndExtra		= 0;
	winclass.hInstance		= hinstance;
	winclass.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
	winclass.hCursor		= LoadCursor(NULL, IDC_ARROW); 
	winclass.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	winclass.lpszMenuName	= NULL;
	winclass.lpszClassName	= WINDOW_CLASS_NAME;
	winclass.hIconSm        = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&winclass))
		return;
	if (windowed)
	{
		main_handler =  CreateWindowEx(NULL,                  // extended style
			WINDOW_CLASS_NAME,     // class
			"DirectDraw Full-Screen Demo", // title
			WS_OVERLAPPED | WS_VISIBLE,
			0,0,	  // initial x,y
			width,height,  // initial width, height
			NULL,	  // handle to parent 
			NULL,	  // handle to menu
			hinstance,// instance of this application
			NULL);
	}else{
		main_handler = CreateWindowEx(NULL,                  // extended style
			WINDOW_CLASS_NAME,     // class
			"DirectDraw Full-Screen Demo", // title
			WS_POPUP | WS_VISIBLE,
			0,0,	  // initial x,y
			width,height,  // initial width, height
			NULL,	  // handle to parent 
			NULL,	  // handle to menu
			hinstance,// instance of this application
			NULL);
	}
	getDevice();
}

void Canvas::getDevice(){
	DD_INIT_STRUCT(ddsd);
	DirectDrawCreateEx(NULL,(LPVOID*)&lp_directdraw,IID_IDirectDraw7,NULL);
	if (is_window)
	{
		lp_directdraw->SetCooperativeLevel(main_handler,DDSCL_NORMAL);
		ddsd.dwFlags = DDSD_CAPS;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
	} 
	else
	{
		lp_directdraw->SetCooperativeLevel(main_handler,DDSCL_FULLSCREEN | DDSCL_ALLOWMODEX | DDSCL_EXCLUSIVE | DDSCL_ALLOWREBOOT);
		lp_directdraw->SetDisplayMode(width,height,bpp,0,0);
		
		ddsd.dwFlags = DDSD_CAPS|DDSD_BACKBUFFERCOUNT;
		ddsd.dwBackBufferCount = 1;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE|DDSCAPS_COMPLEX|DDSCAPS_FLIP;
	}
	lp_directdraw->CreateSurface(&ddsd,&lp_primary_surface,NULL);
	//好像不能直接就取ddsd的lpitch；应该在lock后从ddsd取；
	if(is_window){
		DD_INIT_STRUCT(ddsd);
		ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_SYSTEMMEMORY;
		ddsd.dwWidth = width;
		ddsd.dwHeight = height;
		lp_directdraw->CreateSurface(&ddsd,&lp_back_surface,NULL);
	}else{
		ddsd.ddsCaps.dwCaps = DDSCAPS_BACKBUFFER;
		lp_primary_surface->GetAttachedSurface(&(ddsd.ddsCaps),&lp_back_surface);
	}
}

Canvas::~Canvas(void)
{
}
