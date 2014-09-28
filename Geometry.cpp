#include "Geometry.h"

void transformObject(Object3d* obj,Matrix* m,int type,bool transform_basis,bool transform_normal){
	int i = 0;
	Point3d storage;
	Vector3d result;
	switch (type)
	{
	case TRANSFORM_LOCAL_ONLY:
		for (i = 0;i<obj->num_vertices;i++)
		{
			m->mulPoint3d(&(obj->lp_vertex_local[i].pos),&storage);
			if (transform_normal)
			{
				m->mulVector3d(&(obj->lp_vertex_local[i].normal),&result);
				obj->lp_vertex_local[i].normal.copy(&result);
			}			
			obj->lp_vertex_local[i].pos.copy(&storage);
		}
		break;
	case TRANSFORM_TRANS_ONLY:
		for (i = 0;i<obj->num_vertices;i++)
		{
			m->mulPoint3d(&(obj->lp_vertex_trans[i].pos),&storage);
			if (transform_normal)
			{
				m->mulVector3d(&(obj->lp_vertex_trans[i].normal),&result);
				obj->lp_vertex_trans[i].normal.copy(&result);
			}			
			obj->lp_vertex_trans[i].pos.copy(&storage);
		}
		break;
	case TRANSFORM_LOCAL_TO_TRANS:
		for (i = 0;i<obj->num_vertices;i++)
		{
			m->mulPoint3d(&(obj->lp_vertex_local[i].pos),&storage);
			if (transform_normal)
			{
				m->mulVector3d(&(obj->lp_vertex_local[i].normal),&result);
				obj->lp_vertex_trans[i].normal.copy(&result);
			}			
			obj->lp_vertex_trans[i].pos.copy(&storage);
		}
		break;
	}
	if (transform_basis)
	{
		m->mulVector3d(&(obj->ux),&result);
		obj->ux.copy(&result);
		m->mulVector3d(&(obj->uy),&result);
		obj->uy.copy(&result);
		m->mulVector3d(&(obj->uz),&result);
		obj->uz.copy(&result);
	}
};

Matrix::Matrix(int type){
	width = type%10;
	height = type/10;
	//lp_data = new std::vector<float>(width*height);
};

void  Matrix::init(float* data){
	for (int i = 0;i<width*height;i++)
	{
		lp_data[i] = data[i];
	}
};

int Matrix::multiply(Matrix* m,Matrix* storage){
	if (width != m->height)
	{
		return 0;
	}
	if (storage->width!=m->width||storage->height!=height)
	{
		return 0;
	}
	float temp = 0;
	for (int i = 0;i<height;i++)
	{
		for (int j = 0;j<m->width;j++)
		{
			temp = 0;
			for(int k = 0;k<width;k++)
			{
				temp += lp_data[i*width + k] * (m->lp_data)[k*m->width + j];
			}
			(storage->lp_data)[i*m->width + j] = temp;
		}
	}
	return 1;
};

int Matrix::mulVector3d(Vector3d* vec,Vector3d* storage){
	float temp = 0;
	for (int j = 0;j<width;j++)
	{
		temp = 0;
		for(int k = 0;k<4;k++)
		{
			temp += (vec->M)[k] * lp_data[k*width + j];
		}
		(storage->M)[j] = temp;
	}
	return 1;
}

int Matrix::mulPoint3d(Point3d* vec,Point3d* storage){
	float temp = 0;
	for (int j = 0;j<width;j++)
	{
		temp = 0;
		for(int k = 0;k<4;k++)
		{
			temp += (vec->M)[k] * lp_data[k*width + j];
		}
		(storage->M)[j] = temp;
	}
	return 1;
}

void Matrix::setZero(){
	for (size_t i = 0;i<width*height;i++)
	{
		lp_data[i] = 0;
	}
}

void Matrix::identify(){
	for (size_t i = 0;i<width*height;i++)
	{
		lp_data[i] = 0;
	}
	for (int j = 0;j<width;j++)
	{
		lp_data[j*width + j] = 1;
	}
}

Matrix::~Matrix(){
	//delete lp_data;
};

Object3d::Object3d(){
	state = OBJECT_STATE_ACTIVE|OBJECT_STATE_VISIBLE;
	lp_texture_position_list = NULL;
};

void Object3d::rotationY(float deg){
	float rad = DEG_TO_RAD(deg - ry);
	ry = deg;
	Matrix mx(44);
	float cos_t = cos(rad);
	float sin_t = sin(rad);
	float y_data[16] = {cos_t,0,-sin_t,0,
							0,1,0,0,
							sin_t,0,cos_t,0,
							0,0,0,1};
	mx.init(y_data);
	transformObject(this,&mx,TRANSFORM_LOCAL_ONLY,false,true);
};

void Object3d::rotationX(float deg){
	float rad = DEG_TO_RAD(deg - rx);
	rx = deg;
	Matrix mx(44);
	float cos_t = cos(rad);
	float sin_t = sin(rad);
	float x_data[16] = {1,0,0,0,
						0,cos_t,sin_t,0,
						0,-sin_t,cos_t,0,
						0,0,0,1};
	mx.init(x_data);
	transformObject(this,&mx,TRANSFORM_LOCAL_ONLY,false,true);
};

 void Object3d::rotationZ(float deg){
	float rad = DEG_TO_RAD(deg - rz);
	rz = deg;
	Matrix mx(44);
	float cos_t = cos(rad);
	float sin_t = sin(rad);
	float z_data[16] = {cos_t,sin_t,0,0,
						-sin_t,cos_t,0,0,
						0,0,1,0,
						0,0,0,1};
	mx.init(z_data);
	transformObject(this,&mx,TRANSFORM_LOCAL_ONLY,false,true);
};

void Object3d::scale(float sx,float sy,float sz){
	Vertex3d* temp;
	for (int i = 0;i<num_vertices;i++)
	{
		temp = lp_vertex_local + i;
		temp->x *= sx;
		temp->x *= sy;
		temp->x *= sz;
	}
};

void Object3d::init(int num_v,int num_p,int num_f){
	lp_vertex_local = new Vertex3d[num_v*num_f];
	lp_vertex_trans = new Vertex3d[num_v*num_f];
	lp_polys = new Poly[num_p];
	
	//memset((void*)lp_vertex_local,0,sizeof(Vertex3d)*num_v*num_f);
	//memset((void*)lp_vertex_trans,0,sizeof(Vertex3d)*num_v*num_f);
	//memset((void*)lp_polys,0,sizeof(Poly)*num_p);

	for (int i = 0;i<num_p;i++)
	{
		lp_polys[i].lp_vertex_object = lp_vertex_trans;
	}

	lp_head_vlist_local = lp_vertex_local;
	lp_head_vlist_trans = lp_vertex_trans;

	num_frame = num_f;
	num_polys = num_p;
	num_vertices = num_v;
};


Object3d::~Object3d(){
	delete lp_head_vlist_local;
	delete lp_head_vlist_trans;//lp_head_vlist_local = lp_vertex_local;lp_head_vlist_trans = lp_vertex_trans;所以不释放lp_vertex_local，lp_vertex_trans
	delete lp_polys;
	delete lp_texture_position_list;
	lp_polys = NULL;
	lp_texture_position_list = NULL;
	lp_vertex_local = NULL;
	lp_vertex_trans = NULL;
	lp_head_vlist_local = NULL;
	lp_head_vlist_trans =NULL;
};


void Point3d::build(Point3d* target,Vector3d* storage){
	storage->x = target->x - x;
	storage->y = target->y - y;
	storage->z = target->z - z;
};

void Object3d::setTexture(BitmapData* data){
	texture = data;
	for (int i = 0;i<num_polys;i++)
	{
		lp_polys[i].texture = data;
	}
}