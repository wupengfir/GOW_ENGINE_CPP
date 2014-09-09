#pragma once

#define CLIP_POLY_X_PLANE 0x0001
#define CLIP_POLY_Y_PLANE 0x0002
#define CLIP_POLY_Z_PLANE 0x0004

#define CLIP_CODE_GZ   0x0001    // z > z_max
#define CLIP_CODE_LZ   0x0002    // z < z_min
#define CLIP_CODE_IZ   0x0004    // z_min < z < z_max

#define CLIP_CODE_GX   0x0001    // x > x_max
#define CLIP_CODE_LX   0x0002    // x < x_min
#define CLIP_CODE_IX   0x0004    // x_min < x < x_max

#define CLIP_CODE_GY   0x0001    // y > y_max
#define CLIP_CODE_LY   0x0002    // y < y_min
#define CLIP_CODE_IY   0x0004    // y_min < y < y_max

#define CLIP_CODE_NULL 0x0000

#define TRI_TYPE_NONE 0;
#define TRI_TYPE_TOP 1;
#define TRI_TYPE_BOTTOM 2;
#define TRI_TYPE_GENERAL 3;


#include "Common.h"
class Color;
class BitmapData;
class Vertex3d;
class Poly;
class RenderList;
class Camera;
class RenderPoly;
int Draw_Text_GDI(char *text, int x,int y,COLORREF color = 0xff00ffff);
int drawLine(int x0, int y0, int x1, int y1, UINT color);
int Clip_Line(int &x1,int &y1,int &x2, int &y2);

void Draw_Triangle_2D(float x1,float y1,float x2,float y2,float x3,float y3,int color,UCHAR *dest_buffer, int mempitch);
void Draw_Top_Tri(float x1,float y1, float x2,float y2, float x3,float y3,int color, UCHAR *dest_buffer, int mempitch);
void Draw_Bottom_Tri(float x1,float y1, float x2,float y2, float x3,float y3,int color,UCHAR *dest_buffer, int mempitch);

void Draw_Gouraud_Triangle(float x1,float y1,float x2,float y2,float x3,float y3,Color color0,Color color1,Color color2,UCHAR *dest_buffer, int mempitch);
void Draw_bottom_Gouraud_Triangle(float x1,float y1,float x2,float y2,float x3,float y3,Color color0,Color color1,Color color2,UCHAR *dest_buffer, int mempitch);
void Draw_top_Gouraud_Triangle(float x1,float y1,float x2,float y2,float x3,float y3,Color color0,Color color1,Color color2,UCHAR *dest_buffer, int mempitch);

void drawTextureTriangle(RenderPoly *poly,BitmapData *texture,UCHAR *dest_buffer, int mempitch);
void drawTopTextureTriangle(Vertex3d *v1,Vertex3d *v2,Vertex3d *v3,BitmapData *texture,UCHAR *dest_buffer, int mempitch,Color color);
void drawBottomTextureTriangle(Vertex3d *v1,Vertex3d *v2,Vertex3d *v3,BitmapData *texture,UCHAR *dest_buffer, int mempitch,Color color);

void clipPolyFromRenderlist(RenderList* render_list,Camera* cam,int mode);


void Draw_Triangle_zb(RenderPoly* poly,UCHAR *dest_buffer, int mempitch,float* z_buffer,int z_pitch);
void Draw_Gouraud_Triangle_zb(RenderPoly* poly,UCHAR *dest_buffer, int mempitch,float* z_buffer,int z_pitch);
void drawTextureTriangle_zb(RenderPoly *poly,BitmapData *texture,UCHAR *dest_buffer, int mempitch,float* z_buffer,int z_pitch);