#pragma once
#include "Common.h"

class Color;
class BitmapData;
class Vertex3d;
int Draw_Text_GDI(char *text, int x,int y,COLORREF color = 0xff00ffff);
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

void Draw_Gouraud_Triangle(float x1,float y1,
	float x2,float y2,
	float x3,float y3,
	Color color0,Color color1,Color color2,
	UCHAR *dest_buffer, int mempitch);

void Draw_bottom_Gouraud_Triangle(float x1,float y1,
	float x2,float y2,
	float x3,float y3,
	Color color0,Color color1,Color color2,
	UCHAR *dest_buffer, int mempitch);

void Draw_top_Gouraud_Triangle(float x1,float y1,
	float x2,float y2,
	float x3,float y3,
	Color color0,Color color1,Color color2,
	UCHAR *dest_buffer, int mempitch);

void drawTextureTriangle(Vertex3d *v1,Vertex3d *v2,Vertex3d *v3,BitmapData *texture,UCHAR *dest_buffer, int mempitch);
void drawTopTextureTriangle(Vertex3d *v1,Vertex3d *v2,Vertex3d *v3,BitmapData *texture,UCHAR *dest_buffer, int mempitch);
void drawBottomTextureTriangle(Vertex3d *v1,Vertex3d *v2,Vertex3d *v3,BitmapData *texture,UCHAR *dest_buffer, int mempitch);