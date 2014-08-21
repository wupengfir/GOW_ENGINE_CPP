#include "Loader.h"

void loadObject_ASC(char* path,Object3d* obj,Point3d* p_pos,Vector3d* p_scale,Matrix* p_matrix,int attr){
	std::ifstream fin(path);
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
	while (getline(fin,data))
	{
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
				subStringByMark(data,index+2,*tempString);
				temp_vertex->x = stringToNumber(*tempString);
				subStringByMark(data,indexOf(data,"Y:")+2,*tempString);
				temp_vertex->y = stringToNumber(*tempString);
				subStringByMark(data,indexOf(data,"Z:")+2,*tempString);
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
				subStringByMark(data,index+2,*tempString);
				temp_point->x = stringToNumber(*tempString);
				subStringByMark(data,indexOf(data,"B:")+2,*tempString);
				temp_point->y = stringToNumber(*tempString);
				subStringByMark(data,indexOf(data,"C:")+2,*tempString);
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
		obj->lp_vertex_trans[i].normal.init(0,0,0,1);//这里要处理trans是因为下面算法线用的trans
		obj->lp_vertex_local[i].pos = vertex_list[i]->pos;
		obj->lp_vertex_trans[i].pos = vertex_list[i]->pos;
	}


	//测试纹理，记者删
	obj->lp_texture_position_list = new Point2d[6];
	obj->lp_texture_position_list[0].x = 0;obj->lp_texture_position_list[0].y = 0;
	obj->lp_texture_position_list[1].x = 0;obj->lp_texture_position_list[1].y = 1;
	obj->lp_texture_position_list[2].x = 1;obj->lp_texture_position_list[2].y = 1;
	obj->lp_texture_position_list[3].x = 0;obj->lp_texture_position_list[3].y = 0;
	obj->lp_texture_position_list[4].x = 1;obj->lp_texture_position_list[4].y = 0;
	obj->lp_texture_position_list[5].x = 1;obj->lp_texture_position_list[5].y = 1;
	///////


	for (i = 0;i<num_poly;i++)
	{
		obj->lp_polys[i].v_index_list[0] = (int)pos_List[i]->x;
		obj->lp_polys[i].v_index_list[1] = (int)pos_List[i]->y;
		obj->lp_polys[i].v_index_list[2] = (int)pos_List[i]->z;
		obj->lp_polys[i].color.argb = 0xffffffff;
		obj->lp_polys[i].attr = attr;

		////////////////测试纹理，记者删
		obj->lp_polys[i].lp_texture_position_object = obj->lp_texture_position_list;
		if (i%2 == 0)
		{
			obj->lp_polys[i].t_index_list[0] = 0;
			obj->lp_polys[i].t_index_list[1] = 1;
			obj->lp_polys[i].t_index_list[2] = 2;
		}else{
			obj->lp_polys[i].t_index_list[0] = 3;
			obj->lp_polys[i].t_index_list[1] = 4;
			obj->lp_polys[i].t_index_list[2] = 5;
		}
		//////////////////

	}
	computeVertexNormalVector(obj);
	for (int i = 0;i<vertex_list.size();i++)
	{
		delete vertex_list[i];
	}
	for (int i = 0;i<pos_List.size();i++)
	{
		delete pos_List[i];
	}
	delete tempString;
	fin.close();
};

void loadObject_COB(char* path,Object3d* obj,Point3d* p_pos,Vector3d* p_scale,Matrix* p_matrix,int attr,bool change23){
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
	Point3d* temp_texture_point;
	std::vector<Point3d*> texture_pos_List;
	int index = 0;
	Point3d storage;
	int num_vertex = 0;
	int num_poly = 0;
	int i = 0;
	int start_index;
	Point2d* t_pos;
	if (p_pos)
	{
		obj->world_pos.copy(p_pos);
	}
	while (getline(fin,data))
	{
		trimString(data);
		if (data.empty())
		{
			continue;
		}
		if(data[0] == '#'){
			continue;
		}
		if (startWith(data,"World Vertices"))
		{
			subStringByMark(data,sizeof("World Vertices")/sizeof(char),*tempString);
			int nv = stringToInt(*tempString);
			for (int i = 0;i<nv;i++)
			{
				getline(fin,data);
				trimString(data);

				temp_vertex = new Vertex3d();
				subStringByMark(data,0,*tempString);
				temp_vertex->x = stringToNumber(*tempString);

				start_index = indexOf(data," ");
				subStringByMark(data,start_index+1,*tempString);
				temp_vertex->y = stringToNumber(*tempString);


				subStringByMark(data,indexOf(data," ",start_index + 1)+1,*tempString);
				temp_vertex->z = stringToNumber(*tempString);

				vertex_list.push_back(temp_vertex);
				num_vertex++;
			}
				
		}
		else if (startWith(data,"Texture Vertices"))
		{
			subStringByMark(data,sizeof("Texture Vertices")/sizeof(char),*tempString);
			int nv = stringToInt(*tempString);
			obj->lp_texture_position_list = new Point2d[nv];
			for (int i = 0;i<nv;i++)
			{
				getline(fin,data);
				trimString(data);
				t_pos = new Point2d();
				subStringByMark(data,0,*tempString);
				t_pos->x = stringToNumber(*tempString);

				subStringByMark(data,indexOf(data," ")+1,*tempString);
				t_pos->y = stringToNumber(*tempString);

				obj->lp_texture_position_list[i] = *t_pos;
				delete t_pos;
			}
		}
		else if (startWith(data,"Faces"))
		{
			subStringByMark(data,sizeof("Faces")/sizeof(char),*tempString);
			int nv = stringToInt(*tempString);
			for (int i = 0;i<nv*2;i++)
			{
				getline(fin,data);
				trimString(data);
				if (i%2 == 0)
				{
					continue;
				}
				temp_point = new Point3d();
				start_index = indexOf(data,"<");
				subStringByMark(data,start_index+1,*tempString,',');
				temp_point->x = stringToNumber(*tempString);

				start_index = indexOf(data,"<",start_index + 1);
				subStringByMark(data,start_index+1,*tempString,',');
				temp_point->y = stringToNumber(*tempString);

				start_index = indexOf(data,"<",start_index + 1);
				subStringByMark(data,start_index+1,*tempString,',');
				temp_point->z = stringToNumber(*tempString);
				pos_List.push_back(temp_point);
				num_poly++;

				temp_texture_point = new Point3d();
				start_index = indexOf(data,",");
				subStringByMark(data,start_index+1,*tempString,'>');
				temp_texture_point->x = stringToNumber(*tempString);
				start_index = indexOf(data,",",start_index + 1);
				subStringByMark(data,start_index+1,*tempString,'>');
				temp_texture_point->y = stringToNumber(*tempString);
				start_index = indexOf(data,",",start_index + 1);
				subStringByMark(data,start_index+1,*tempString,'>');
				temp_texture_point->z = stringToNumber(*tempString);
				texture_pos_List.push_back(temp_texture_point);
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
		obj->lp_vertex_trans[i].normal.init(0,0,0,1);//这里要处理trans是因为下面算法线用的trans
		obj->lp_vertex_local[i].pos = vertex_list[i]->pos;
		obj->lp_vertex_trans[i].pos = vertex_list[i]->pos;
	}

	for (i = 0;i<num_poly;i++)
	{
		if (change23)
		{
			obj->lp_polys[i].v_index_list[2] = (int)pos_List[i]->x;
			obj->lp_polys[i].v_index_list[1] = (int)pos_List[i]->y;
			obj->lp_polys[i].v_index_list[0] = (int)pos_List[i]->z;

			obj->lp_polys[i].lp_texture_position_object = obj->lp_texture_position_list;
			obj->lp_polys[i].t_index_list[2] = (int)texture_pos_List[i]->x;
			obj->lp_polys[i].t_index_list[1] = (int)texture_pos_List[i]->y;
			obj->lp_polys[i].t_index_list[0] = (int)texture_pos_List[i]->z;
		}else{
			obj->lp_polys[i].v_index_list[0] = (int)pos_List[i]->x;
			obj->lp_polys[i].v_index_list[1] = (int)pos_List[i]->y;
			obj->lp_polys[i].v_index_list[2] = (int)pos_List[i]->z;

			obj->lp_polys[i].lp_texture_position_object = obj->lp_texture_position_list;
			obj->lp_polys[i].t_index_list[0] = (int)texture_pos_List[i]->x;
			obj->lp_polys[i].t_index_list[1] = (int)texture_pos_List[i]->y;
			obj->lp_polys[i].t_index_list[2] = (int)texture_pos_List[i]->z;
		}
		

		obj->lp_polys[i].color.argb = 0xffffffff;
		obj->lp_polys[i].attr = attr;

	}
	computeVertexNormalVector(obj);
	for (int i = 0;i<vertex_list.size();i++)
	{
		delete vertex_list[i];
	}
	for (int i = 0;i<pos_List.size();i++)
	{
		delete pos_List[i];
	}
	for (int i = 0;i<texture_pos_List.size();i++)
	{
		delete texture_pos_List[i];
	}
	delete tempString;
	fin.close();
}

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


void loadBitmapImage(BitmapData *bitmapdata,char* path){

	char* temp_data;
	bool flip = false;
	std::ifstream fin(path,std::ios::binary);
	fin.seekg(sizeof(BITMAPFILEHEADER),std::ios::beg);
	BITMAPINFOHEADER header;
	fin.read((char*)&header,sizeof(header));
	bitmapdata->width = header.biWidth;
	bitmapdata->height = header.biHeight;
	flip = bitmapdata->height>0?true:false;
	bitmapdata->height = abs(bitmapdata->height);
	if ((bitmapdata->width&(bitmapdata->width -1)) == 0)
	{
		int power = 1;
		while(bitmapdata->width>>power != 1){
			power++;
		}
		bitmapdata->power_of_two = power;
	}
	bitmapdata->data = new Color[bitmapdata->width*bitmapdata->height];
	if (header.biBitCount != 24)
	{
		return;
		//fin.seekg(sizeof(RGBQUAD),std::ios::cur);
	}
	UINT tempdata = 0;
	int lineLength = (bitmapdata->width*3 + 3)/4*4;
	temp_data = new char[lineLength*bitmapdata->height];
	fin.read(temp_data,lineLength*bitmapdata->height);
	for (int i = 0;i<bitmapdata->height;i++)
	{
		for (int j = 0;j<bitmapdata->width;j++)
		{
			if (flip)
			{
				bitmapdata->data[(bitmapdata->height - i - 1)*bitmapdata->width+j].b = (UCHAR)temp_data[i*lineLength + j*3];
				bitmapdata->data[(bitmapdata->height - i - 1)*bitmapdata->width+j].g = (UCHAR)temp_data[i*lineLength + j*3 + 1];
				bitmapdata->data[(bitmapdata->height - i - 1)*bitmapdata->width+j].r = (UCHAR)temp_data[i*lineLength + j*3 + 2];
			}else{
				bitmapdata->data[i*bitmapdata->width+j].b = (UCHAR)temp_data[i*lineLength + j*3];
				bitmapdata->data[i*bitmapdata->width+j].g = (UCHAR)temp_data[i*lineLength + j*3 + 1];
				bitmapdata->data[i*bitmapdata->width+j].r = (UCHAR)temp_data[i*lineLength + j*3 + 2];
			}			
		}
	}
	delete[] temp_data;
};

BitmapData::~BitmapData(){
	delete[] data;
};