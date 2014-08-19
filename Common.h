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
extern Canvas* lp_canvas;


inline void trimString(std::string& str){
	str.erase(str.find_last_not_of(" ") + 1);
	str.erase(0,str.find_first_not_of(" "));
};

inline bool startWith(std::string& str,const char* find){
	int index = 0;
	while(find[index] != '\0'){
		if(str[index] != find[index]){
			return false;
		}
		index++;
	}
	return true;
};

inline int indexOf(std::string& str,const char* find,int start_index = 0){
	int temp = -1;
	bool testing = false;
	int i = 0;
	if (start_index >= str.size())
	{
		return -1;
	}
	for (std::string::size_type index = start_index;index!=str.size();index++)
	{
		if (str[index] == find[0])
		{
			i = 1;
			temp = index;
			while (find[i] != '\0')
			{
				if ((index + i)==str.size())
				{
					temp = -1;
					return -1;
				}
				if (str[index + i] != find[i])
				{
					temp = -1;
					break;
				}
				i++;
			}
			if (temp != -1)
			{
				return temp;
			}
		}else{
			temp = -1;
		}
	}
	return temp;
};

inline std::string& subStringByBlank(std::string& str,int start_index,std::string& storage){
	int index = start_index;
	int num = 0;
	int length = 3;
	char* buffer = new char[length];
	char* temp = NULL;
	while(index<str.size()&&str[index] != ' '){
		if (num == length)
		{
			length *= 2;
			temp = new char[length];
			memcpy(temp,buffer,length);
			delete buffer;
			buffer = temp;
			temp = NULL;
		}
		buffer[num] = str[index];
		index++;
		num++;
	}
	if (num == length)
	{
		length++;
		temp = new char[length];
		memcpy(temp,buffer,length);
		delete buffer;
		buffer = temp;
		temp = NULL;
	}
	buffer[num] = '\0';
	storage = buffer;
	delete buffer;
	return storage;
};

inline float stringToNumber(std::string& str){
	float num = 0;
	std::stringstream stream(str);
	stream>>num;
	return num;
}
//template<typename T> 
//inline std::string numberToString(T a){
//	std::string s;
//	std::stringstream stream(T);
//	stream>>s;
//	return s;
//};

template<typename T> 
void releaseT(T a) 
{
	if(a == NULL)return;
	a->Release();
	a = NULL;
} 


