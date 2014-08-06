#include "Geometry.h"
Matrix::Matrix(int type){
	width = type%10;
	height = type/10;
	lp_data = new std::vector<float>(width*height);
};
Matrix::~Matrix(){
};
