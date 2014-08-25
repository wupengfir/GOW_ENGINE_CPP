#include "Common.h"
int min_clip_x,                             // clipping rectangle 
	max_clip_x,
	min_clip_y,
	max_clip_y;

void trimString(std::string& str){
	str.erase(str.find_last_not_of(" ") + 1);
	str.erase(0,str.find_first_not_of(" "));
};

bool startWith(std::string& str,const char* find){
	int index = 0;
	while(find[index] != '\0'){
		if(str[index] != find[index]){
			return false;
		}
		index++;
	}
	return true;
};

int indexOf(std::string& str,const char* find,int start_index){
	int temp = -1;
	bool testing = false;
	int i = 0;
	if (start_index >= str.size())
	{
		return -1;
	}
	for (std::string::size_type index = start_index;index!=str.size();index++)
	{
		if (str[index] == find[0])
		{
			i = 1;
			temp = index;
			while (find[i] != '\0')
			{
				if ((index + i)==str.size())
				{
					temp = -1;
					return -1;
				}
				if (str[index + i] != find[i])
				{
					temp = -1;
					break;
				}
				i++;
			}
			if (temp != -1)
			{
				return temp;
			}
		}else{
			temp = -1;
		}
	}
	return temp;
};

std::string& subStringByMark(std::string& str,int start_index,std::string& storage,char mark){
	int index = start_index;
	int num = 0;
	int length = 10;
	char* buffer = new char[length];
	char* temp = NULL;
	while(index<str.size()&&str[index] != mark){
		//如果长度不够，再分配;
		if (num == length)
		{
			length *= 2;
			temp = new char[length];
			memcpy(temp,buffer,length);
			delete buffer;
			buffer = temp;
			temp = NULL;
		}
		buffer[num] = str[index];
		index++;
		num++;
	}
	if (num == length)
	{
		length++;
		temp = new char[length];
		memcpy(temp,buffer,length);
		delete buffer;
		buffer = temp;
		temp = NULL;
	}
	buffer[num] = '\0';
	storage = buffer;
	delete buffer;
	return storage;
};

void quickSort(std::vector<RenderPoly*>& data,int begin,int end){
	if (begin >= end)
	{
		return;
	}
	RenderPoly* k = data[begin];
	int i = begin;
	int j = end;
	while(i<j){
		while(data[j]->avg_z<=k->avg_z&&i<j){
			j--;
		}
		data[i] = data[j];
		while(data[i]->avg_z>=k->avg_z&&i<j){
			i++;
		}
		data[j] = data[i];
	}
	data[i] = k;
	quickSort(data,begin,i-1);
	quickSort(data,i+1,end);
}