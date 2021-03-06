#pragma once
#include "Common.h"
#include "Geometry.h"
#include "GraphicFunctions.h"
#include "Light.h"
#include "Loader.h"
inline void Mem_Set_QUAD(void *dest, UINT data, int count)
{
	_asm 
	{ 
		mov edi, dest   ; edi points to destination memory
		mov ecx, count  ; number of 32-bit words to move
		mov eax, data   ; 32-bit data
		rep stosd       ; move data
	} 
};

class ZBuffer{
public:
	static const int bit_16 = 0;
	static const int bit_32 = 1;
	int attr;
	int width;
	int height;
	float* buffer;

	void clear(float data = 10000000.0);
	ZBuffer(int width,int height,int attr = 1);
	~ZBuffer(void);
};

class Camera;
class Point2d;
class Point3d;
class Vector3d;
class Vertex3d;
class Poly;
class Plane3d;
class Object3d;
class RenderList;
class Matrix;
class Light;
class BitmapData;
class Canvas
{
public:
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
	Camera* lp_camera;
	RenderList* renderlist_all;
	ZBuffer *z_buffer;
	UINT *alpha_table;
	std::vector<Object3d*> obj_list;
	std::vector<RenderList*> renderlist_list;
	std::vector<Light*> LightList;
public:
	void init(HINSTANCE hinstance,WNDPROC callback,int width,int height,int bpp,int windowed);
	UCHAR* lock();
	void unlock();
	void flip();
	void clear();
	void reset();
	void getDevice();
	void createAlphaTable();
	Canvas(void);
	~Canvas(void);

	void setCamera(Camera* camera);
	void addObject(Object3d* obj);
	void addRenderList(RenderList* list);
	void lightObject(Object3d* obj);
	void lightRenderlist(RenderList* list);
	void render(bool backmove = true,bool cull = true);

	inline void addLight(Light* light){
		LightList.push_back(light);
	};

	inline void plotPixel(int x,int y,UINT color){
		//((UINT*)lp_backbuffer)[x+((y*lpitch)>>2)] = color; 
		Mem_Set_QUAD((UINT*)lp_backbuffer + x+((y*lpitch)>>2),color,1);
	}

	void showImage(BitmapData* bitmapdata,int x,int y);

};

