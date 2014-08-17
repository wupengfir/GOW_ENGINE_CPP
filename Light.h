#pragma once
#include "Common.h"
#include "Geometry.h"

class Point3d;
class Vector3d;
class Color;
class Light{
public:
	static const int LIGHTV1_ATTR_AMBIENT = 0x0001;
	static const int LIGHTV1_ATTR_INFINITE = 0x0002;
	static const int LIGHTV1_ATTR_POINT = 0x0004;
	static const int LIGHTV1_ATTR_SPOTLIGHT_SIMPLE = 0x0008;
	static const int LIGHTV1_ATTR_SPOTLIGHT_COMPLICATE = 0x0010;
	static const int LIGHTV1_STATE_ON = 1;
	static const int LIGHTV1_STATE_OFF = 0;

	int state;
	int id;
	std::string name;
	int attr;

	Color *c_ambient;
	Color *c_diffuse;
	Color *c_specular;
	Point3d *pos;
	Vector3d *dir;
	float kc;
	float kl;
	float kq;
	float spot_inner;
	float spot_outer;
	float pf;

	Light(void){};
	~Light(void);

	void init(int state,int attr,
				Color *c_ambient,Color *c_diffuse,Color *c_specular,
				float kc,float kl,float kq,
				Point3d* pos,Vector3d* dir,
				float spot_inner,float spot_outer,float pf);
};