#pragma once
#define WIN32_LEAN_AND_MEAN  // just say no to MFC

#define INITGUID // make sure directX guids are included

// MACROS /////////////////////////////////////////////////

#define PI 3.14159
#define DEG_TO_RAD(a) ((a)/180*PI)
#define KEYDOWN(vk_code) ((GetAsyncKeyState(vk_code) & 0x8000) ? 1 : 0)
#define KEYUP(vk_code)   ((GetAsyncKeyState(vk_code) & 0x8000) ? 0 : 1)
#define _RGB32BIT(a,r,g,b) ((b) + ((g) << 8) + ((r) << 16) + ((a) << 24))
#define DD_INIT_STRUCT(ddstruct) { memset(&ddstruct,0,sizeof(ddstruct)); ddstruct.dwSize=sizeof(ddstruct); }
#define FCMP(a,b) ((a>b?(a-b):(b-a))<0.001)
#define DELETE_BASETYPE_POINTER(p) {delete p;p = NULL;}

#include <windows.h>   // include important windows stuff
#include <windowsx.h> 
#include <mmsystem.h>
#include <fstream>
#include <sstream>
#include <conio.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <string>
#include <stdarg.h>
#include <stdio.h> 
#include <math.h>
#include <io.h>
#include <fcntl.h>
#include <vector>
#include <ddraw.h> // include directdraw


#include "Canvas.h"
//#include "GraphicFunctions.h"
//#include "Geometry.h"
//#include "Render.h"
#pragma comment( lib,"winmm.lib" )
#pragma comment(lib,"ddraw.lib")
// DEFINES ////////////////////////////////////////////////

// defines for windows 
#define WINDOW_CLASS_NAME "WINCLASS1"

// default screen size
#define SCREEN_WIDTH    640  // size of screen
#define SCREEN_HEIGHT   480
#define SCREEN_BPP      32    // bits per pixel
#define MAX_COLORS      256  // maximum colors

// TYPES //////////////////////////////////////////////////////

// basic unsigned types
typedef unsigned short USHORT;
typedef unsigned short WORD;
typedef unsigned char  UCHAR;
typedef unsigned char  BYTE;




extern int min_clip_x,                             // clipping rectangle 
	max_clip_x,
	min_clip_y,
	max_clip_y;

class Canvas;
class Poly;
extern Canvas* lp_canvas;


std::string& subStringByMark(std::string& str,int start_index,std::string& storage,char mark = ' ');
int indexOf(std::string& str,const char* find,int start_index = 0);
bool startWith(std::string& str,const char* find);
void trimString(std::string& str);

inline float stringToNumber(std::string& str){
	float num = 0;
	std::stringstream stream(str);
	stream>>num;
	return num;
}

inline int stringToInt(std::string& str){
	int num = 0;
	std::stringstream stream(str);
	stream>>num;
	return num;
}

void quickSort(std::vector<Poly*>& data,int begin,int end);

template<typename T> 
void releaseT(T a) 
{
	if(a == NULL)return;
	a->Release();
	a = NULL;
} 


