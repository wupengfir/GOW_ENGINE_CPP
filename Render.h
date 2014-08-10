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

	  Camera(void);
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