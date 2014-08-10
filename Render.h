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