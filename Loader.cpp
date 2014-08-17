#include "Loader.h"

void loadObject_ASC(char* path,Object3d* obj,Point3d* p_pos,Vector3d* p_scale,Matrix* p_matrix){
	std::ifstream fin(path);
	char buffer[1024];
	char number[10];
	std::string data;
	std::stringstream transfer;
	std::string* tempString = new std::string();
	Vertex3d* temp_vertex;
	std::vector<Vertex3d*> vertex_list;
	Point3d* temp_point;
	std::vector<Point3d*> pos_List;
	int index = 0;
	Point3d storage;
	int num_vertex = 0;
	int num_poly = 0;
	int i = 0;
	if (p_pos)
	{
		obj->world_pos.copy(p_pos);
	}
	while (fin.getline(buffer,1024))
	{
		data = buffer;
		trimString(data);
		if (data.empty())
		{
			continue;
		}
		if(data[0] == '#'){
			continue;
		}
		if (startWith(data,"Vertex"))
		{
			index = indexOf(data,"X:");
			if (index != -1)
			{
				temp_vertex = new Vertex3d();
				subStringByBlank(data,index+2,*tempString);
				temp_vertex->x = stringToNumber(*tempString);
				subStringByBlank(data,indexOf(data,"Y:")+2,*tempString);
				temp_vertex->y = stringToNumber(*tempString);
				subStringByBlank(data,indexOf(data,"Z:")+2,*tempString);
				temp_vertex->z = stringToNumber(*tempString);
				vertex_list.push_back(temp_vertex);
				num_vertex++;
			}
		}
		else if (startWith(data,"Face"))
		{
			index = indexOf(data,"A:");
			if (index != -1)
			{
				temp_point = new Point3d();
				subStringByBlank(data,index+2,*tempString);
				temp_point->x = stringToNumber(*tempString);
				subStringByBlank(data,indexOf(data,"B:")+2,*tempString);
				temp_point->y = stringToNumber(*tempString);
				subStringByBlank(data,indexOf(data,"C:")+2,*tempString);
				temp_point->z = stringToNumber(*tempString);
				pos_List.push_back(temp_point);
				num_poly++;
			}
		}
	}
	obj->init(num_vertex,num_poly,1);
	for (i = 0;i<num_vertex;i++)
	{
		if (p_matrix)
		{
			p_matrix->mulPoint3d(&(vertex_list[i]->pos),&storage);
			vertex_list[i]->pos = storage;
		}
		if (p_scale)
		{
			vertex_list[i]->pos.x *= p_scale->x;
			vertex_list[i]->pos.y *= p_scale->y;
			vertex_list[i]->pos.z *= p_scale->z;
		}
		obj->lp_vertex_local[i].normal.init(0,0,0,1);
		obj->lp_vertex_trans[i].normal.init(0,0,0,1);
		obj->lp_vertex_local[i].pos = vertex_list[i]->pos;
		obj->lp_vertex_trans[i].pos = vertex_list[i]->pos;
	}
	for (i = 0;i<num_poly;i++)
	{
		obj->lp_polys[i].v_index_list[0] = (int)pos_List[i]->x;
		obj->lp_polys[i].v_index_list[1] = (int)pos_List[i]->y;
		obj->lp_polys[i].v_index_list[2] = (int)pos_List[i]->z;
		obj->lp_polys[i].color.argb = 0xffffffff;
		obj->lp_polys[i].attr = POLY4D_ATTR_SHADE_MODE_GOURAUD;
	}
	computeVertexNormalVector(obj);
	delete temp_point;
	delete temp_vertex;
	delete tempString;
	fin.close();
};

void computeVertexNormalVector(Object3d* obj){
	if (obj == NULL)
	{
		return;
	}
	Poly* lp_poly = NULL;
	Vertex3d* lp_vertex = NULL;
	for (int i = 0;i<obj->num_polys;i++)
	{
		lp_poly = obj->lp_polys + i;
		lp_poly->calculateNormalVectorWithoutNormallize();
		lp_vertex = obj->lp_vertex_local + lp_poly->v_index_list[0];
		lp_vertex->normal.add(&lp_poly->normal_vector,&lp_vertex->normal);
		lp_vertex = obj->lp_vertex_local + lp_poly->v_index_list[1];
		lp_vertex->normal.add(&lp_poly->normal_vector,&lp_vertex->normal);
		lp_vertex = obj->lp_vertex_local + lp_poly->v_index_list[2];
		lp_vertex->normal.add(&lp_poly->normal_vector,&lp_vertex->normal);
	}
	for (int index = 0;index<obj->num_vertices;index++)
	{
		lp_vertex = obj->lp_vertex_local + index;
		normalizeVector3d(&lp_vertex->normal);
	}
};