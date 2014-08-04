#pragma once
#include "InitD3D.h"
class Canvas
{
public:
	LPDIRECTDRAW7		 lp_directdraw;
	LPDIRECTDRAWSURFACE7 lp_primary_surface;
	LPDIRECTDRAWSURFACE7 lp_back_surface;
public:
	UCHAR* lp_bitmapdata;
	int width;
	int height;
	DDSURFACEDESC2 ddsd;
	int is_window;
	int bpp;
	HINSTANCE main_instance;
	HWND main_handler;
public:
	void init(HINSTANCE hinstance,WNDPROC callback,int width,int height,int bpp,int windowed);
	void flip();
	void clear();
	void reset();
	void getDevice();
	Canvas(void);
	~Canvas(void);
};

