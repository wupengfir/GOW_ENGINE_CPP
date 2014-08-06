#pragma once
#include "Common.h"

#define ALMOST_ZERO						0.00001;

#define RENDERLISTD_MAX_POLYS			8192;
#define OBJECT4D_MAX_VERTICES			1024;

#define POLY4D_ATTR_2SIDES				0x0001;
#define POLY4D_ATTR_TRANSPARENT			0x0002;
#define POLY4D_ATTR_8BITCOLOR			0x0004;
#define POLY4D_ATTR_RGB16				0x0008;
#define POLY4D_ATTR_RGB24				0x0010;		
#define POLY4D_ATTR_SHADE_MODE_PURE		0x0020;
#define POLY4D_ATTR_SHADE_MODE_FLAT		0x0040;
#define POLY4D_ATTR_SHADE_MODE_GOURAUD	0x0080;
#define POLY4D_ATTR_SHADE_MODE_PHONG	0x0100;
#define POLY4D_ATTR_SHADE_MODE_TEXTURE	0x0200;

#define POLY4D_ATTR_ENABLE_MATERIAL		0x0800;
#define POLY4D_ATTR_DISABLE_MATERIAL	0x1000;

#define POLY4D_STATE_NULLE				0x0000;
#define POLY4D_STATE_ACTIVE				0x0001;
#define POLY4D_STATE_CLIPPED			0x0002;
#define POLY4D_STATE_BACKFACE			0x0004;
#define POLY4D_STATE_LIT				0x0008;

#define OBJECT_STATE_NULL				0x0000;
#define OBJECT_STATE_ACTIVE				0x0001;
#define OBJECT_STATE_CLIPPED			0x0002;
#define OBJECT_STATE_VISIBLE			0x0004;

#define OBJECT_ATTR_SINGLE_FRAME		0x0001;
#define OBJECT_ATTR_MULTI_FRAME			0x0002;
#define OBJECT_ATTR_TEXTURE				0x0004;

#define VERTEX_ATTR_NULL				0x0000;
#define VERTEX_ATTR_POINT				0x0001;
#define VERTEX_ATTR_NORMAL				0x0002;
#define VERTEX_ATTR_TEXTURE				0x00004;

#define TRANSFORM_LOCAL_ONLY			0;
#define TRANSFORM_TRANS_ONLY			1;
#define TRANSFORM_LOCAL_TO_TRANS		2;

#define CULL_X							0x0001;
#define CULL_Y							0x0002;
#define CULL_Z							0x0004;
#define CULL_XYZ						0x0007;

#define UNV_MODE_SIMPLE					0;
#define UNV_MODE_SPHERICAL				1;

#define SWAP(a,b,t) {t=a; a=b; b=t;}
class Point2d 
{
public:
	union{
		int M[2];
		struct{
			int x;
			int y;
		};
	};	
};

class Point3d 
{
public:
	union{
		int M[4];
		struct{
			int x;
			int y;
			int z;
			int w;
		};
	};		
};

class Vector3d 
{
public:
	union{
		int M[4];
		struct{
			int x;
			int y;
			int z;
			int w;
		};
	};		
};

class Vertex3d{
public:
	union{
		float M[12];
		struct{
			float x,y,z,w;
			float nx,ny,nz,nw;
			float tx,ty;
			UINT color;
			int attr;
		};
		struct{
			Point3d pos;
			Vector3d normal;
			Point2d texture_pos;
		};
	};

};

class Poly{
public:
	int state;
	int attr;
	UINT color;
	UINT lit_color[3];

	int matrial_index;
	float normal_length;
	float avg_z;

	Vertex3d vlist_local[3];
	Vertex3d vlist_trans[3];
};

class BaseObject3d{
public:
	int id;
	std::string name;
	int state;
	int attr;
	int matrial_index;
	float avg_radius;
	float max_radius;

	Point3d world_pos;
	Vector3d dir;
	Vector3d ux,uy,uz;

	int num_vertices;
	int num_frame;
	int curr_frame;
	int num_polys;

	Vertex3d* lp_vertex_local;
	Vertex3d* lp_vertex_trans;

	Vertex3d* lp_head_vlist_local;
	Vertex3d* lp_head_vlist_trans;

	Poly* lp_polys;
	int setFrame(int frame);
};

class RenderList{
public:
	int state;
	int attr;
	int num_polys;
	std::vector<Poly> polys;
};

class Matrix{
public:
	int width;
	int height;
	std::vector<float>* lp_data;
	Matrix(int type);
	~Matrix();


};

inline void vector3dCopy(Vector3d* vdst, Vector3d* vsrc)
{ *vdst = *vsrc; }

inline void vector3dInit(Vector3d* vdst)
{ 
	vdst->x = 0;
	vdst->y = 0;
	vdst->z = 0;
	vdst->w = 0;
}

inline float VECTOR4D_Length_Fast2(Vector3d* va)
{
	int temp;  // used for swaping
	int x,y,z; // used for algorithm
	x = fabs((float)va->x) * 1024;
	y = fabs((float)va->y) * 1024;
	z = fabs((float)va->z) * 1024;
	if (y < x) SWAP(x,y,temp)
	if (z < y) SWAP(y,z,temp)
	if (y < x) SWAP(x,y,temp)
	int dist = (z + 11 * (y >> 5) + (x >> 2) );
	return((float)(dist >> 10));

}