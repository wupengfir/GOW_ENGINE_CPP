#pragma once
#include "Common.h"

int drawLine(int x0, int y0, int x1, int y1, UINT color);
int Clip_Line(int &x1,int &y1,int &x2, int &y2);
void Draw_Triangle_2D(int x1,int y1,
	int x2,int y2,
	int x3,int y3,
	int color,
	UCHAR *dest_buffer, int mempitch);
void Draw_Top_Tri(int x1,int y1, 
	int x2,int y2, 
	int x3,int y3,
	int color, 
	UCHAR *dest_buffer, int mempitch);
void Draw_Bottom_Tri(int x1,int y1, 
	int x2,int y2, 
	int x3,int y3,
	int color,
	UCHAR *dest_buffer, int mempitch);