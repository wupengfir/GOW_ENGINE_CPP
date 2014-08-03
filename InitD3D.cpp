#include "InitD3D.h"


InitD3D::InitD3D(void)
{
}

int InitD3D::add(int a,int b){
	return a+b;
}

//LRESULT CALLBACK WindowProc(HWND hwnd, 
//	UINT msg, 
//	WPARAM wparam, 
//	LPARAM lparam)
//{
//	// this is the main message handler of the system
//	PAINTSTRUCT		ps;		// used in WM_PAINT
//	HDC				hdc;	// handle to a device context
//	char buffer[80];        // used to print strings
//
//	// what is the message 
//	switch(msg)
//	{	
//	case WM_CREATE: 
//		{
//			// do initialization stuff here
//			// return success
//			return(0);
//		} break;
//
//	case WM_PAINT: 
//		{
//			// simply validate the window 
//			hdc = BeginPaint(hwnd,&ps);	 
//
//			// end painting
//			EndPaint(hwnd,&ps);
//
//			// return success
//			return(0);
//		} break;
//
//	case WM_DESTROY: 
//		{
//
//			// kill the application, this sends a WM_QUIT message 
//			PostQuitMessage(0);
//
//			// return success
//			return(0);
//		} break;
//
//	default:break;
//
//	} // end switch
//
//	// process any messages that we didn't take care of 
//	return (DefWindowProc(hwnd, msg, wparam, lparam));
//
//} // end WinProc

HWND InitD3D::createWindow(HINSTANCE hinstance,WNDPROC callback,int width,int height){
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
}

int InitD3D::initDDraw(LPDIRECTDRAW7* lpdd,HWND handler,DWORD type,DDSURFACEDESC2* ddsd,LPDIRECTDRAWSURFACE7* lpsurface){
	if (FAILED(DirectDrawCreateEx(NULL,(LPVOID*)lpdd,IID_IDirectDraw7,NULL)))
	{
		return 0;
	}
	if(FAILED((*lpdd)->SetCooperativeLevel(handler,type))){
		return 0;
	}
	memset(ddsd,0,sizeof(*ddsd));
	ddsd->dwSize = sizeof(*ddsd);
	ddsd->dwFlags = DDSD_CAPS;
	ddsd->ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
	if (FAILED((*lpdd)->CreateSurface(ddsd,lpsurface,NULL)))
	{
		return 0;
	}
	//if (FAILED((*lpdd)->SetDisplayMode(1366,768,32,0,0)))
	//{
	//	return 0;
	//}
	return 1;
}

InitD3D::~InitD3D(void)
{
}
