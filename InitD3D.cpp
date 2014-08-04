#include "InitD3D.h"


InitD3D::InitD3D(void)
{
}

int InitD3D::add(int a,int b){
	return a+b;
}

HWND InitD3D::createWindow(HINSTANCE hinstance,WNDPROC callback,int width,int height,int windowed){
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
		return(0);
	if (windowed)
	{
		return CreateWindowEx(NULL,                  // extended style
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
		return CreateWindowEx(NULL,                  // extended style
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
	
}

//int InitD3D::initDDraw_v2(HWND handler,int width,int height,int bpp,int windowed){
//	lp_canva->reset();
//	lp_canva->getDevice(handler,width,height,bpp,windowed);
//	return 1;
//}

int InitD3D::initDDraw(LPDIRECTDRAW7* lpdd,HWND handler,DWORD type,DDSURFACEDESC2* ddsd,LPDIRECTDRAWSURFACE7* lpsurface,LPDIRECTDRAWSURFACE7* lpsurfaceb_back){
	memset(ddsd,0,sizeof(*ddsd));
	ddsd->dwSize = sizeof(*ddsd);
	if (FAILED(DirectDrawCreateEx(NULL,(LPVOID*)lpdd,IID_IDirectDraw7,NULL)))
	{
		return 0;
	}
	if(FAILED((*lpdd)->SetCooperativeLevel(handler,type))){
		return 0;
	}
	if (type&DDSCL_FULLSCREEN)
	{
		if (FAILED((*lpdd)->SetDisplayMode(SCREEN_WIDTH,SCREEN_HEIGHT,SCREEN_BPP,0,0)))
		{
			return 0;
		}
	}
	if (lpsurfaceb_back)
	{
		ddsd->dwFlags = DDSD_CAPS|DDSD_BACKBUFFERCOUNT;
		ddsd->dwBackBufferCount = 1;
		ddsd->ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE|DDSCAPS_COMPLEX|DDSCAPS_FLIP;
	}else{
		ddsd->dwFlags = DDSD_CAPS;
		ddsd->ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
	}	
	
	if (FAILED((*lpdd)->CreateSurface(ddsd,lpsurface,NULL)))
	{
		return 0;
	}
	if (lpsurfaceb_back)
	{
		ddsd->ddsCaps.dwCaps = DDSCAPS_BACKBUFFER;
		if (FAILED((*lpsurface)->GetAttachedSurface(&(ddsd->ddsCaps),lpsurfaceb_back)))
		{
			return 0;
		}
	}
	
	return 1;
}

InitD3D::~InitD3D(void)
{
}
