#include "Geometry.h"
Camera::Camera():mcam(44),mper(44),mscr(44){

};

Camera::~Camera()
{

};

void Camera::init(int attr,Point3d cam_pos,Vector3d cam_dir,Point3d cam_target,
	float nearZ,float farZ,float fov,float viewW,float viewH){
		this->attr = attr;
		pos = cam_pos;
		dir = cam_dir;
		near_clip_z = nearZ;
		far_clip_z = farZ;
		this->fov = fov;
		viewport_width = viewW;
		viewport_height = viewH;
		u.init(1,0,0,1);
		v.init(0,1,0,1);
		n.init(0,0,1,1);
		target = cam_target;
		viewport_centerX = viewW/2;
		viewport_centerY = viewH/2;
		aspect_radio = viewW/viewH;
		viewplane_width = 2;
		viewplane_height = 2/aspect_radio;
		float tan_fov = tan(DEG_TO_RAD(fov/2));
		view_dist = 1/tan_fov;
		Point3d p;p.init(0,0,0);
		Vector3d vn;
		if (fov == 90.0)
		{
			vn.init(1,0,-1);
			rt_clip_z.p = p;
			rt_clip_z.v = vn;
			vn.init(-1,0,-1);
			lt_clip_z.p = p;
			lt_clip_z.v = vn;
			vn.init(0,1,-1);
			tp_clip_z.p = p;
			tp_clip_z.v = vn;
			vn.init(0,-1,-1);
			bt_clip_z.p = p;
			bt_clip_z.v = vn;
		} 
		else
		{
			vn.init(view_dist,0,viewplane_width/(-2));
			rt_clip_z.p = p;
			rt_clip_z.v = vn;
			vn.init(-view_dist,0,viewplane_width/(-2));
			lt_clip_z.p = p;
			lt_clip_z.v = vn;
			vn.init(0,view_dist,viewplane_width/(-2));
			tp_clip_z.p = p;
			tp_clip_z.v = vn;
			vn.init(0,-view_dist,viewplane_width/(-2));
			bt_clip_z.p = p;
			bt_clip_z.v = vn;
		}
};

void Camera::buildWorldToCameraMatrix_Euler(){
	Matrix mt_inv(44);
	Matrix mx_inv(44);
	Matrix my_inv(44);
	Matrix mz_inv(44);
	float t_data[16] = {1,0,0,0,
						0,1,0,0,
						0,0,1,0,
						-pos.x,-pos.y,-pos.z,1};
	mt_inv.init(t_data);
	float t_x = dir.x;
	float t_y = dir.y;
	float t_z = dir.z;
	float cos_t = cos(t_x);
	float sin_t = -sin(t_x);
	float x_data[16] = {1,0,0,0,
						0,cos_t,sin_t,0,
						0,-sin_t,cos_t,0,
						0,0,0,1};
	cos_t = cos(t_y);
	sin_t = -sin(t_y);
	float y_data[16] = {cos_t,0,-sin_t,0,
						0,1,0,0,
						sin_t,0,cos_t,0,
						0,0,0,1};
	cos_t = cos(t_z);
	sin_t = -sin(t_z);
	float z_data[16] = {cos_t,sin_t,0,0,
						-sin_t,cos_t,0,0,
						0,0,1,0,
						0,0,0,1};
	mx_inv.init(x_data);
	my_inv.init(y_data);
	mz_inv.init(z_data);

	Matrix storage_yz(44);
	Matrix storage_yzx(44);
	my_inv.multiply(&mz_inv,&storage_yz);
	storage_yz.multiply(&mx_inv,&storage_yzx);
	mt_inv.multiply(&storage_yzx,&mcam);
};

void Camera::buildWorldToCameraMatrix_unv(int mode){
	Matrix mt_inv(44);
	float t_data[16] = {1,0,0,0,
						0,1,0,0,
						0,0,1,0,
						-pos.x,-pos.y,-pos.z,1};
	mt_inv.init(t_data);
	n.build(&pos,&target);
	v.init(0,1,0,1);
	v.cross(&n,&u);
	n.cross(&u,&v);
	normalizeVector3d(&u);
	normalizeVector3d(&v);
	normalizeVector3d(&n);
	float data[16] = {u.x,v.x,n.x,0,
						u.y,v.y,n.y,0,
						u.z,v.z,n.z,0,
						0,0,0,1};
	Matrix temp(44);
	temp.init(data);
	mt_inv.multiply(&temp,&mcam);
};

void Camera::removeBackFaceOfObj(Object3d *obj){
	Vector3d view_vector;
	Poly* lp_p = obj->lp_polys;
	for (int i = 0;i<obj->num_polys;i++)
	{	
		if (lp_p->attr & POLY4D_ATTR_2SIDES)
		{
			lp_p++;
			continue;
		}
		view_vector.build(&(lp_p->lp_vertex_object[lp_p->v_index_list[0]].pos),&pos);
		lp_p->calculateNormalVector();
		float temp = view_vector.pointMultiply(&lp_p->normal_vector);
		if (temp > 0)
		{
			//lp_p->color = 0xffff0000;
			lp_p->state = POLY4D_STATE_ACTIVE;
		} 
		else
		{
			//lp_p->color = 0xff00ff00;
			lp_p->state = POLY4D_STATE_BACKFACE;
		}
		lp_p++;
	}
};

void Camera::cameraToPerspective_renderlist(RenderList *list){
	Poly* lp_p = 0;
	std::vector<Poly*>::iterator iter = list->polys.begin();
	for (;iter!=list->polys.end();iter++)
	{
		lp_p = *iter;
		if (lp_p != NULL&&lp_p->avaliable())
		{
			for (int i = 0;i<3;i++)
			{
				float tz = lp_p->lp_vertex_object[lp_p->v_index_list[i]].z;
				lp_p->lp_vertex_object[lp_p->v_index_list[i]].x *= view_dist/tz;
				lp_p->lp_vertex_object[lp_p->v_index_list[i]].y *= view_dist*aspect_radio/tz;
			}
		}
	}
};

void Camera::cameraToPerspective_object(Object3d *obj){
	Vertex3d* lp_p;
	for (int i = 0;i<obj->num_vertices;i++)
	{
		lp_p = obj->lp_vertex_trans + i;
		if (lp_p != NULL)
		{
				float tz = lp_p->z;
				lp_p->x *= view_dist/tz;
				lp_p->y *= view_dist*aspect_radio/tz;
		}
	}
};
void Camera::cullObject(Object3d* obj,int type){
	obj->state = OBJECT_STATE_ACTIVE;
	Point3d temp;
	mcam.mulPoint3d(&obj->world_pos,&temp);
	if(type & CULL_Z){
		if((temp.z - obj->max_radius)>far_clip_z||(temp.z + obj->max_radius)<near_clip_z){
			obj->state = OBJECT_STATE_CLIPPED;
		}
	}
	if(type & CULL_X){
		float x_test = 0.5*viewplane_width*temp.z/view_dist;
		if((temp.x - obj->max_radius)>x_test||(temp.x + obj->max_radius)<-x_test){
			obj->state = OBJECT_STATE_CLIPPED;
		}
	}
	if(type & CULL_Y){
		float y_test = 0.5*viewplane_height*temp.z/view_dist;
		if((temp.y - obj->max_radius)>y_test||(temp.y + obj->max_radius)<-y_test){
			obj->state = OBJECT_STATE_CLIPPED;
		}
	}
};

void Camera::perspectiveToScreen_renderlist(RenderList* list){
	Poly* lp_p = 0;
	std::vector<Poly*>::iterator iter = list->polys.begin();
	float a = 0.5*viewport_width-0.5;
	float b = 0.5*viewport_height-0.5;
	for (;iter!=list->polys.end();iter++)
	{
		lp_p = *iter;
		if (lp_p != NULL&&lp_p->avaliable())
		{			
			for (int i = 0;i<3;i++)
			{
				lp_p->lp_vertex_object[lp_p->v_index_list[i]].x = a+a*lp_p->lp_vertex_object[lp_p->v_index_list[i]].x;
				lp_p->lp_vertex_object[lp_p->v_index_list[i]].y = b-b*lp_p->lp_vertex_object[lp_p->v_index_list[i]].y;
			}
		}
	}
};

void Camera::perspectiveToScreen_object(Object3d *obj){
	Vertex3d* lp_p;
	float a = 0.5*viewport_width-0.5;
	float b = 0.5*viewport_height-0.5;
	for (int i = 0;i<obj->num_vertices;i++)
	{
		lp_p = obj->lp_vertex_trans + i;
		if (lp_p != NULL)
		{
			lp_p->x = a+a*lp_p->x;
			lp_p->y = b-b*lp_p->y;
			
		}
	}
};
