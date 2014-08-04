#pragma once
#define WIN32_LEAN_AND_MEAN  // just say no to MFC

#define INITGUID // make sure directX guids are included

#include <windows.h>   // include important windows stuff
#include <windowsx.h> 
#include <mmsystem.h>
#include <iostream> // include important C/C++ stuff
#include <conio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <string.h>
#include <stdarg.h>
#include <stdio.h> 
#include <math.h>
#include <io.h>
#include <fcntl.h>

#include <ddraw.h> // include directdraw

#include "Point2d.h"
#include "Canvas.h"
#pragma comment(lib,"ddraw.lib")
// DEFINES ////////////////////////////////////////////////

// defines for windows 
#define WINDOW_CLASS_NAME "WINCLASS1"

// default screen size
#define SCREEN_WIDTH    1600  // size of screen
#define SCREEN_HEIGHT   900
#define SCREEN_BPP      32    // bits per pixel
#define MAX_COLORS      256  // maximum colors

// TYPES //////////////////////////////////////////////////////

// basic unsigned types
typedef unsigned short USHORT;
typedef unsigned short WORD;
typedef unsigned char  UCHAR;
typedef unsigned char  BYTE;

// MACROS /////////////////////////////////////////////////

#define KEYDOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEYUP(vk_code)   ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)
#define _RGB32BIT(a,r,g,b) ((b) + ((g) << 8) + ((r) << 16) + ((a) << 24))
// initializes a direct draw struct
#define DD_INIT_STRUCT(ddstruct) { memset(&ddstruct,0,sizeof(ddstruct)); ddstruct.dwSize=sizeof(ddstruct); }
class Canvas;
class InitD3D
{
public:
	Point2d p;
	Canvas *lp_canva;
public:
	InitD3D(void);
	int initDDraw_v2(HWND handler,int width,int height,int bpp,int windowed);
	HWND createWindow(HINSTANCE hinstance,WNDPROC callback,int width,int height,int windowed = 0);
	int initDDraw(LPDIRECTDRAW7* lpdd,HWND handler,DWORD type,DDSURFACEDESC2* ddsd,LPDIRECTDRAWSURFACE7* lpsurface,LPDIRECTDRAWSURFACE7* lpsurfaceb_back = NULL);
	int add(int a,int b);
	~InitD3D(void);
};

