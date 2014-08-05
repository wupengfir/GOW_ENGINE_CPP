#pragma once
#include "Common.h"
class Canvas
{
private:
	LPDIRECTDRAW7		 lp_directdraw;
	LPDIRECTDRAWSURFACE7 lp_primary_surface;
	LPDIRECTDRAWSURFACE7 lp_back_surface;
public:
	UCHAR* lp_backbuffer;
	int lpitch;
	int width;
	int height;
	DDSURFACEDESC2 ddsd;
	int is_window;
	int bpp;
	HINSTANCE main_instance;
	HWND main_handler;
	int window_client_x0;
	int window_client_y0;
public:
	void init(HINSTANCE hinstance,WNDPROC callback,int width,int height,int bpp,int windowed);
	UCHAR* lock();
	void unlock();
	void flip();
	void clear();
	void reset();
	void getDevice();
	Canvas(void);
	~Canvas(void);

	inline void plotPixel(int x,int y,UINT color){
		((UINT*)lp_backbuffer)[x+y*lpitch/4] = color; 
	}

};

