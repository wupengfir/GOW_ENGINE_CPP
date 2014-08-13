#pragma once
#include "Common.h"

int drawLine(int x0, int y0, int x1, int y1, UINT color);
int Clip_Line(int &x1,int &y1,int &x2, int &y2);
void Draw_Triangle_2D(float x1,float y1,
	float x2,float y2,
	float x3,float y3,
	int color,
	UCHAR *dest_buffer, int mempitch);
void Draw_Top_Tri(float x1,float y1, 
	float x2,float y2, 
	float x3,float y3,
	int color, 
	UCHAR *dest_buffer, int mempitch);
void Draw_Bottom_Tri(float x1,float y1, 
	float x2,float y2, 
	float x3,float y3,
	int color,
	UCHAR *dest_buffer, int mempitch);