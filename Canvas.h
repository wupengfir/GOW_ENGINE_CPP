#pragma once
#include "Common.h"
#include "Geometry.h"
#include "GraphicFunctions.h"
//#include "Render.h"

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
	Camera* lp_camera;
	RenderList* renderlist_all;
	std::vector<Object3d*> obj_list;
	std::vector<RenderList*> renderlist_list;
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

	void setCamera(Camera* camera);
	void addObject(Object3d* obj);
	void addRenderList(RenderList* list);
	void shaderObject(Object3d* obj);
	void render(bool backmove = true,bool cull = true);
	inline void plotPixel(int x,int y,UINT color){
		((UINT*)lp_backbuffer)[x+((y*lpitch)>>2)] = color; 
	}

};

