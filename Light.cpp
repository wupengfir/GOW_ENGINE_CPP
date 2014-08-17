#include "Light.h"

void Light::init(int state,int attr, Color *c_ambient,Color *c_diffuse,Color *c_specular, float kc,float kl,float kq, Point3d* ppos,Vector3d* pdir, float spot_inner,float spot_outer,float pf){
	this->pos = new Point3d();
	this->dir = new Vector3d();
	this->state = state;
	this->attr = attr;
	this->c_ambient = c_ambient;
	this->c_diffuse = c_diffuse;
	this->c_specular = c_specular;
	this->kc = kc;
	this->kl = kl;
	this->kq = kq;
	if(ppos != NULL){
		this->pos->copy(ppos);
		delete ppos;
	}		
	if(pdir != NULL){
		this->dir->copy(pdir);
		normalizeVector3d(this->dir);
		delete pdir;
	}			
	this->spot_inner = spot_inner;
	this->spot_outer = spot_outer;
	this->pf = pf;
};

Light::~Light(void){
	DELETE_BASETYPE_POINTER(c_diffuse);
	DELETE_BASETYPE_POINTER(c_ambient);
	DELETE_BASETYPE_POINTER(c_specular);
	DELETE_BASETYPE_POINTER(pos);
	DELETE_BASETYPE_POINTER(dir);
};