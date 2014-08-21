#pragma once
#include "Common.h"
#include "Geometry.h"

class BitmapData{
public:
	int width;
	int height;
	int power_of_two;
	Color *data;

	BitmapData(){
		width = 0;
		height = 0;
		power_of_two = -1;
		data  = NULL;
	};
	~BitmapData();
};

class Object3d;
class Matrix;
void loadObject_ASC(char* path,Object3d* obj,Point3d* p_pos,Vector3d* p_scale,Matrix* p_matrix,int attr);
void loadObject_COB(char* path,Object3d* obj,Point3d* p_pos,Vector3d* p_scale,Matrix* p_matrix,int attr,bool change23 = false);
void computeVertexNormalVector(Object3d* obj);
//只加载24位的
void loadBitmapImage(BitmapData *bitmapdata,char* path);