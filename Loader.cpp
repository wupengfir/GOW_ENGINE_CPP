#include "Loader.h"

void loadObject_ASC(char* path,Object3d* obj,Point3d* p_pos,Vector3d* p_scale,Matrix* p_matrix){
	std::ifstream fin(path);
	char buffer[1024];
	char number[10];
	std::string data;
	std::stringstream transfer;
	std::string temp;
	int index = 0;
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
				subStringByBlank(data,index+2,temp);
			}
		}
	}
	fin.close();
};