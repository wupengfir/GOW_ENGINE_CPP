#pragma once
#include "Common.h"
#define ALMOST_ZERO						0.00001

#define RENDERLISTD_MAX_POLYS			8192
#define OBJECT4D_MAX_VERTICES			1024

#define POLY4D_ATTR_2SIDES				0x0001
#define POLY4D_ATTR_TRANSPARENT			0x0002
#define POLY4D_ATTR_8BITCOLOR			0x0004
#define POLY4D_ATTR_RGB16				0x0008
#define POLY4D_ATTR_RGB24				0x0010		
#define POLY4D_ATTR_SHADE_MODE_PURE		0x0020
#define POLY4D_ATTR_SHADE_MODE_FLAT		0x0040
#define POLY4D_ATTR_SHADE_MODE_GOURAUD	0x0080
#define POLY4D_ATTR_SHADE_MODE_PHONG	0x0100
#define POLY4D_ATTR_SHADE_MODE_TEXTURE	0x0200

#define POLY4D_ATTR_ENABLE_MATERIAL		0x0800
#define POLY4D_ATTR_DISABLE_MATERIAL	0x1000

#define POLY4D_STATE_NULLE				0x0000
#define POLY4D_STATE_ACTIVE				0x0001
#define POLY4D_STATE_CLIPPED			0x0002
#define POLY4D_STATE_BACKFACE			0x0004
#define POLY4D_STATE_LIT				0x0008

#define OBJECT_STATE_NULL				0x0000
#define OBJECT_STATE_ACTIVE				0x0001
#define OBJECT_STATE_CLIPPED			0x0002
#define OBJECT_STATE_VISIBLE			0x0004

#define OBJECT_ATTR_SINGLE_FRAME		0x0001
#define OBJECT_ATTR_MULTI_FRAME			0x0002
#define OBJECT_ATTR_TEXTURE				0x0004

#define VERTEX_ATTR_NULL				0x0000
#define VERTEX_ATTR_POINT				0x0001
#define VERTEX_ATTR_NORMAL				0x0002
#define VERTEX_ATTR_TEXTURE				0x00004

#define TRANSFORM_LOCAL_ONLY			0
#define TRANSFORM_TRANS_ONLY			1
#define TRANSFORM_LOCAL_TO_TRANS		2

#define CULL_X							0x0001
#define CULL_Y							0x0002
#define CULL_Z							0x0004
#define CULL_XYZ						0x0007

#define UNV_MODE_SIMPLE					0
#define UNV_MODE_SPHERICAL				1

#define SWAP(a,b,t) {t=a; a=b; b=t;}


class Vector3d;
class Object3d;
class Point3d;
class Matrix;
class Color;
inline void vector3dCopy(Vector3d* vdst, Vector3d* vsrc);
inline void vector3dInit(Vector3d* vdst);
inline float vector3dLengthFast(Vector3d* va);
inline void normalizeVector3d(Vector3d* v);
inline float point_distance(Point3d* p1,Point3d* p2);
void transformObject(Object3d* obj,Matrix* m,int type,bool transform_basis,bool transform_normal);

class Color{
public:
	union{
		UINT argb;
		UCHAR M[4];
		struct{
			UCHAR b;
			UCHAR g;
			UCHAR r;
			UCHAR a;
		};
	};
	inline Color* init(UINT cr,UINT cg,UINT cb,UINT ca = 0xff){
		a = ca;
		r = cr;
		g = cg;
		b = cb;
		return this;
	}; 

	inline Color* init(UINT argb){
		this->argb = argb;
		return this;
	}; 
};

class Point2d 
{
public:
	union{
		float M[2];
		struct{
			float x;
			float y;
		};
	};	
};

class Point3d 
{
public:
	union{
		float M[4];
		struct{
			float x;
			float y;
			float z;
			float w;
		};
	};	
	inline void init(float vx,float vy,float vz,float vw = 1){
		x = vx;
		y = vy;
		z = vz;
		w = vw;
	};

	inline void add(Point3d* target,Point3d* storage){
		storage->x = x + target->x;
		storage->y = y + target->y;
		storage->z = z + target->z;
	};

	inline void copy(Point3d *source){
		x = source->x;
		y = source->y;
		z = source->z;
		w = source->w;
	}

	Point3d(){
		w = 1;
	};

};

class Vector3d 
{
public:
	union{
		float M[4];
		struct{
			float x;
			float y;
			float z;
			float w;
		};
	};	
	inline void init(float vx,float vy,float vz,float vw = 1){
		x = vx;
		y = vy;
		z = vz;
		w = vw;
	};

	inline void add(Vector3d* target,Vector3d* storage){
		storage->x = x + target->x;
		storage->y = y + target->y;
		storage->z = z + target->z;
	};

	inline void cross(Vector3d* target,Vector3d* storage){
		storage->x = y*target->z-z*target->y;
		storage->y = target->x*z - x*target->z;
		storage->z = x*target->y - y*target->x;
	};

	inline void build(Point3d* source,Point3d* target){
		x = target->x - source->x;
		y = target->y - source->y;
		z = target->z - source->z;
	};

	inline float pointMultiply(Vector3d* target){
		return x*target->x + y*target->y + z*target->z;
	};

	inline void copy(Vector3d *source){
		x = source->x;
		y = source->y;
		z = source->z;
		w = source->w;
	}

};

class Vertex3d{
public:
	union{
		float M[12];
		struct{
			float x,y,z,w;
			float nx,ny,nz,nw;
			float tx,ty;
			Color color;
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
	Color color;
	Color lit_color[3];

	int matrial_index;
	float normal_length;
	float avg_z;

	Vector3d normal_vector;

	Vertex3d* lp_vertex_object;

	int v_index_list[3];

	Poly(){
		state = POLY4D_STATE_ACTIVE;
		attr = 0;
	}

	inline bool avaliable(){
		if(!(state&POLY4D_STATE_ACTIVE)|| state&POLY4D_STATE_CLIPPED || state&POLY4D_STATE_BACKFACE)
			return false;
		return true;
	};

	inline void calculateAvgZ(){
		avg_z = (lp_vertex_object[v_index_list[0]].z+lp_vertex_object[v_index_list[1]].z+lp_vertex_object[v_index_list[2]].z)/3;
	}

	inline void calculateNormalVector(){

		calculateNormalVectorWithoutNormallize();
		normalizeVector3d(&normal_vector);
	}

	inline  void calculateNormalVectorWithoutNormallize(){

		float x1 = lp_vertex_object[v_index_list[1]].x-lp_vertex_object[v_index_list[0]].x;
		float y1 = lp_vertex_object[v_index_list[1]].y-lp_vertex_object[v_index_list[0]].y;
		float z1 = lp_vertex_object[v_index_list[1]].z-lp_vertex_object[v_index_list[0]].z;
		Vector3d u;
		u.x = x1;u.y = y1;u.z = z1;

		float x2 = lp_vertex_object[v_index_list[2]].x-lp_vertex_object[v_index_list[0]].x;
		float y2 = lp_vertex_object[v_index_list[2]].y-lp_vertex_object[v_index_list[0]].y;
		float z2 = lp_vertex_object[v_index_list[2]].z-lp_vertex_object[v_index_list[0]].z;
		Vector3d v;
		v.x = x2;v.y = y2;v.z = z2;

		normal_vector.x = u.y*v.z-v.y*u.z;
		normal_vector.y = u.z*v.x-u.x*v.z;
		normal_vector.z = u.x*v.y-v.x*u.y;
		normal_vector.w = 1;
	}

};

class Plane3d{
public:
	Point3d p;
	Vector3d v;
};

class Object3d{
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

	//int init_num_polys;
	//int init_num_vertexs;

	Vertex3d* lp_vertex_local;
	Vertex3d* lp_vertex_trans;

	Vertex3d* lp_head_vlist_local;
	Vertex3d* lp_head_vlist_trans;

	Poly* lp_polys;

	float rx,ry,rz;

	Object3d(void);
	~Object3d();
	void scale(float x,float y,float z);	
	int setFrame(int frame);
	void init(int num_v,int num_p,int num_f);

	inline void toWorldPosition(int type){
		if (type == TRANSFORM_LOCAL_TO_TRANS)
		{
			for (int i = 0;i<num_vertices;i++)
			{
				lp_vertex_local[i].pos.add(&world_pos,&(lp_vertex_trans[i].pos));
				lp_vertex_trans[i].normal.copy(&(lp_vertex_local[i].normal));
			}
		} 
		else if(type == TRANSFORM_TRANS_ONLY)
		{
			for (int i = 0;i<num_vertices;i++)
			{
				lp_vertex_trans[i].pos.add(&world_pos,&(lp_vertex_trans[i].pos));
			}
		}
	};

	inline bool avaliable(){
		if(!(state&OBJECT_STATE_ACTIVE)|| state&OBJECT_STATE_CLIPPED || !(state&OBJECT_STATE_VISIBLE)){
			return false;
		}			
		return true;
	};

	//inline void addVertex(Vertex3d *v){
	//	lp_vertex_local[num_vertices] = *v;
	//	lp_vertex_trans[num_vertices] = *v;
	//	num_vertices++;
	//};

	inline void addPoly(int p1,int p2,int p3){
		Poly* temp = new Poly;
		temp->v_index_list[0] = p1;
		temp->v_index_list[1] = p2;
		temp->v_index_list[2] = p3;
		temp->lp_vertex_object = lp_vertex_trans;
		lp_polys[num_polys] = *temp;
		num_polys++;
		delete temp;
		temp = NULL;
	};

	//inline void rotationX(float deg){
	//	float rad = DEG_TO_RAD(deg - rx);
	//	rx = deg;
	//	Matrix mx(44);
	//	float cos_t = cos(rad);
	//	float sin_t = sin(rad);
	//	float x_data[16] = {1,0,0,0,
	//						0,cos_t,-sin_t,0,
	//						0,sin_t,cos_t,0,
	//						0,0,0,1};
	//	mx.init(x_data);
	//	transformObject(this,&mx,TRANSFORM_LOCAL_ONLY,false,true);
	//};

	void rotationY(float deg);

	//inline void rotationZ(float deg){
	//	float rad = DEG_TO_RAD(deg - rz);
	//	rz = deg;
	//	Matrix mx(44);
	//	float cos_t = cos(rad);
	//	float sin_t = sin(rad);
	//	float z_data[16] = {cos_t,-sin_t,0,0,
	//						sin_t,cos_t,0,0,
	//						0,0,1,0,
	//						0,0,0,1};
	//	mx.init(z_data);
	//	transformObject(this,&mx,TRANSFORM_LOCAL_ONLY,false,true);
	//};

};

class RenderList{
public:
	int state;
	int attr;
	int num_polys;
	std::vector<Poly*> polys;
	inline void reset(){
		num_polys = 0;
	};

	inline void addPoly(Poly* poly){
		if(polys.capacity() == 0){
			polys.resize(1024);
		}
		else if (polys.capacity() == num_polys)
		{
			polys.resize(polys.capacity()*2);
		}
		polys[num_polys] = poly;
		num_polys++;
	}
};

class Matrix{
public:
	int width;
	int height;
	//这里要改用数组静态分配内存，记着
	float lp_data[16];

	int multiply(Matrix* m,Matrix* storage);
	int mulVector3d(Vector3d* vec,Vector3d* storage);
	int mulPoint3d(Point3d* p,Point3d* storage);
	void init(float* data);
	void setZero();
	void identify();
	Matrix(int type);
	~Matrix();
};

class Camera{
public:
	static const int CAMERA_TYPE_EULER = 0;
	static const int CAMERA_TYPE_UVN = 1;

	int state;
	int attr; 
	Point3d pos;
	Vector3d dir;
	Vector3d u;
	Vector3d v;
	Vector3d n;
	Point3d target;
	//焦点视距
	float view_dist;
	//视野
	float fov;
	//裁剪面
	float near_clip_z;
	float far_clip_z;

	Plane3d rt_clip_z;
	Plane3d lt_clip_z;

	Plane3d tp_clip_z;
	Plane3d bt_clip_z;
	//视平面宽高
	float viewplane_width;
	float viewplane_height;

	//视口属性
	float viewport_width;
	float viewport_height;
	float viewport_centerX;
	float viewport_centerY;

	//宽高比
	float aspect_radio;
	//相机从世界坐标到相机坐标变换矩阵
	Matrix mcam;
	//相机从相机坐标到透视坐标变换矩阵
	Matrix mper;
	//相机从透视坐标到屏幕坐标变换矩阵
	Matrix mscr;

	Camera(void);
	~Camera(void);
	void init(int attr,Point3d cam_pos,Vector3d cam_dir,Point3d cam_target,
		float nearZ,float farZ,float fov,float viewW,float viewH);

	void buildWorldToCameraMatrix_Euler();
	void buildWorldToCameraMatrix_unv(int mode);
	void removeBackFaceOfObj(Object3d *obj);
	void cameraToPerspective_renderlist(RenderList *list);
	void cameraToPerspective_object(Object3d *obj);
	void cullObject(Object3d* obj,int type = CULL_XYZ);
	void perspectiveToScreen_renderlist(RenderList* list);
	void perspectiveToScreen_object(Object3d *obj);
};

inline void vector3dCopy(Vector3d* vdst, Vector3d* vsrc)
{ *vdst = *vsrc; };

inline void vector3dInit(Vector3d* vdst)
{ 
	vdst->x = 0;
	vdst->y = 0;
	vdst->z = 0;
	vdst->w = 0;
};

inline float vector3dLengthFast(Vector3d* va)
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

};

inline void normalizeVector3d(Vector3d* v){
	float length = vector3dLengthFast(v);
	v->x /= length;
	v->y /= length;
	v->z /= length;
};

inline float point_distance(Point3d* p1,Point3d* p2){
	Vector3d v;
	v.build(p1,p2);
	return vector3dLengthFast(&v);
};