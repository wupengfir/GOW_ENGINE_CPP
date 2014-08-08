#include "Common.h"

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
	//�����Ӿ�
	  float view_dist;
	//��Ұ
	  float fov;
	//�ü���
	  float near_clip_z;
	  float far_clip_z;

	  Plane3d rt_clip_z;
	  Plane3d lt_clip_z;

	  Plane3d tp_clip_z;
	  Plane3d bt_clip_z;
	//��ƽ����
	  float viewplane_width;
	  float viewplane_height;

	//�ӿ�����
	  float viewport_width;
	  float viewport_height;
	  float viewport_centerX;
	  float viewport_centerY;

	//��߱�
	  float aspect_radio;
	//������������굽�������任����
	  Matrix mcam;
	//�����������굽͸������任����
	  Matrix mper;
	//�����͸�����굽��Ļ����任����
	  Matrix mscr;
};