#include "GraphicFunctions.h"

int Draw_Text_GDI(char *text, int x,int y,COLORREF color)
{
	// this function draws the sent text on the sent surface 
	// using color index as the color in the palette

	HDC xdc; // the working dc

	// get the dc from surface
	if (FAILED(lp_canvas->lp_back_surface->GetDC(&xdc)))
		return(0);

	// set the colors for the text up
	SetTextColor(xdc,color);

	// set background mode to transparent so black isn't copied
	SetBkMode(xdc, TRANSPARENT);

	// draw the text a
	TextOut(xdc,x,y,text,strlen(text));

	// release the dc
	lp_canvas->lp_back_surface->ReleaseDC(xdc);

	// return success
	return(1);
}

int drawLine(int x0, int y0, int x1, int y1, UINT color)
{
	if (!Clip_Line(x0,y0,x1,y1))
	{
		return 0;
	}	
	int x, y, dx, dy, dx2, dy2, xstep, ystep, error, index;
	x = x0;
	y = y0;
	dx = x1 - x0;
	dy = y1 - y0;

	if (dx >= 0) // 从左往右画
	{
		xstep = 1; // x步进正1
	}
	else // 从右往左画
	{
		xstep = -1; // x步进负1
		dx = -dx; // 取绝对值
	}

	if (dy >= 0) // 从上往下画
	{
		ystep = 1; // y步进正1
	}
	else // 从下往上画
	{ 
		ystep = -1; // y步进负1
		dy = -dy; // 取绝对值
	}

	dx2 = dx << 1; // 2 * dx
	dy2 = dy << 1; // 2 * dy

	if (dx > dy) // 近X轴直线
	{
		error = dy2 - dx;
		for (index = 0; index <= dx; ++index)
		{
			lp_canvas->plotPixel(x, y, color);
			if (error >= 0)
			{
				error -= dx2;
				y += ystep;
			}
			error += dy2;
			x += xstep;
		}
	}
	else // 近Y轴直线
	{
		error = dx2 - dy;
		for (index = 0; index <= dy; ++index)
		{
			lp_canvas->plotPixel(x, y, color);
			if (error >= 0)
			{
				error -= dy2;
				x += xstep;
			}
			error += dx2;
			y += ystep;
		}
	}

	return 1;
}

int Clip_Line(int &x1,int &y1,int &x2, int &y2)
{
	// this function clips the sent line using the globally defined clipping
	// region

	// internal clipping codes
#define CLIP_CODE_C  0x0000
#define CLIP_CODE_N  0x0008
#define CLIP_CODE_S  0x0004
#define CLIP_CODE_E  0x0002
#define CLIP_CODE_W  0x0001

#define CLIP_CODE_NE 0x000a
#define CLIP_CODE_SE 0x0006
#define CLIP_CODE_NW 0x0009 
#define CLIP_CODE_SW 0x0005

	int xc1=x1, 
		yc1=y1, 
		xc2=x2, 
		yc2=y2;

	int p1_code=0, 
		p2_code=0;

	// determine codes for p1 and p2
	if (y1 < min_clip_y)
		p1_code|=CLIP_CODE_N;
	else
		if (y1 > max_clip_y)
			p1_code|=CLIP_CODE_S;

	if (x1 < min_clip_x)
		p1_code|=CLIP_CODE_W;
	else
		if (x1 > max_clip_x)
			p1_code|=CLIP_CODE_E;

	if (y2 < min_clip_y)
		p2_code|=CLIP_CODE_N;
	else
		if (y2 > max_clip_y)
			p2_code|=CLIP_CODE_S;

	if (x2 < min_clip_x)
		p2_code|=CLIP_CODE_W;
	else
		if (x2 > max_clip_x)
			p2_code|=CLIP_CODE_E;

	// try and trivially reject
	if ((p1_code & p2_code)) 
		return(0);

	// test for totally visible, if so leave points untouched
	if (p1_code==0 && p2_code==0)
		return(1);

	// determine end clip point for p1
	switch(p1_code)
	{
	case CLIP_CODE_C: break;

	case CLIP_CODE_N:
		{
			yc1 = min_clip_y;
			xc1 = x1 + 0.5+(min_clip_y-y1)*(x2-x1)/(y2-y1);
		} break;
	case CLIP_CODE_S:
		{
			yc1 = max_clip_y;
			xc1 = x1 + 0.5+(max_clip_y-y1)*(x2-x1)/(y2-y1);
		} break;

	case CLIP_CODE_W:
		{
			xc1 = min_clip_x;
			yc1 = y1 + 0.5+(min_clip_x-x1)*(y2-y1)/(x2-x1);
		} break;

	case CLIP_CODE_E:
		{
			xc1 = max_clip_x;
			yc1 = y1 + 0.5+(max_clip_x-x1)*(y2-y1)/(x2-x1);
		} break;

		// these cases are more complex, must compute 2 intersections
	case CLIP_CODE_NE:
		{
			// north hline intersection
			yc1 = min_clip_y;
			xc1 = x1 + 0.5+(min_clip_y-y1)*(x2-x1)/(y2-y1);

			// test if intersection is valid, of so then done, else compute next
			if (xc1 < min_clip_x || xc1 > max_clip_x)
			{
				// east vline intersection
				xc1 = max_clip_x;
				yc1 = y1 + 0.5+(max_clip_x-x1)*(y2-y1)/(x2-x1);
			} // end if

		} break;

	case CLIP_CODE_SE:
		{
			// south hline intersection
			yc1 = max_clip_y;
			xc1 = x1 + 0.5+(max_clip_y-y1)*(x2-x1)/(y2-y1);	

			// test if intersection is valid, of so then done, else compute next
			if (xc1 < min_clip_x || xc1 > max_clip_x)
			{
				// east vline intersection
				xc1 = max_clip_x;
				yc1 = y1 + 0.5+(max_clip_x-x1)*(y2-y1)/(x2-x1);
			} // end if

		} break;

	case CLIP_CODE_NW: 
		{
			// north hline intersection
			yc1 = min_clip_y;
			xc1 = x1 + 0.5+(min_clip_y-y1)*(x2-x1)/(y2-y1);

			// test if intersection is valid, of so then done, else compute next
			if (xc1 < min_clip_x || xc1 > max_clip_x)
			{
				xc1 = min_clip_x;
				yc1 = y1 + 0.5+(min_clip_x-x1)*(y2-y1)/(x2-x1);	
			} // end if

		} break;

	case CLIP_CODE_SW:
		{
			// south hline intersection
			yc1 = max_clip_y;
			xc1 = x1 + 0.5+(max_clip_y-y1)*(x2-x1)/(y2-y1);	

			// test if intersection is valid, of so then done, else compute next
			if (xc1 < min_clip_x || xc1 > max_clip_x)
			{
				xc1 = min_clip_x;
				yc1 = y1 + 0.5+(min_clip_x-x1)*(y2-y1)/(x2-x1);	
			} // end if

		} break;

	default:break;

	} // end switch

	// determine clip point for p2
	switch(p2_code)
	{
	case CLIP_CODE_C: break;

	case CLIP_CODE_N:
		{
			yc2 = min_clip_y;
			xc2 = x2 + (min_clip_y-y2)*(x1-x2)/(y1-y2);
		} break;

	case CLIP_CODE_S:
		{
			yc2 = max_clip_y;
			xc2 = x2 + (max_clip_y-y2)*(x1-x2)/(y1-y2);
		} break;

	case CLIP_CODE_W:
		{
			xc2 = min_clip_x;
			yc2 = y2 + (min_clip_x-x2)*(y1-y2)/(x1-x2);
		} break;

	case CLIP_CODE_E:
		{
			xc2 = max_clip_x;
			yc2 = y2 + (max_clip_x-x2)*(y1-y2)/(x1-x2);
		} break;

		// these cases are more complex, must compute 2 intersections
	case CLIP_CODE_NE:
		{
			// north hline intersection
			yc2 = min_clip_y;
			xc2 = x2 + 0.5+(min_clip_y-y2)*(x1-x2)/(y1-y2);

			// test if intersection is valid, of so then done, else compute next
			if (xc2 < min_clip_x || xc2 > max_clip_x)
			{
				// east vline intersection
				xc2 = max_clip_x;
				yc2 = y2 + 0.5+(max_clip_x-x2)*(y1-y2)/(x1-x2);
			} // end if

		} break;

	case CLIP_CODE_SE:
		{
			// south hline intersection
			yc2 = max_clip_y;
			xc2 = x2 + 0.5+(max_clip_y-y2)*(x1-x2)/(y1-y2);	

			// test if intersection is valid, of so then done, else compute next
			if (xc2 < min_clip_x || xc2 > max_clip_x)
			{
				// east vline intersection
				xc2 = max_clip_x;
				yc2 = y2 + 0.5+(max_clip_x-x2)*(y1-y2)/(x1-x2);
			} // end if

		} break;

	case CLIP_CODE_NW: 
		{
			// north hline intersection
			yc2 = min_clip_y;
			xc2 = x2 + 0.5+(min_clip_y-y2)*(x1-x2)/(y1-y2);

			// test if intersection is valid, of so then done, else compute next
			if (xc2 < min_clip_x || xc2 > max_clip_x)
			{
				xc2 = min_clip_x;
				yc2 = y2 + 0.5+(min_clip_x-x2)*(y1-y2)/(x1-x2);	
			} // end if

		} break;

	case CLIP_CODE_SW:
		{
			// south hline intersection
			yc2 = max_clip_y;
			xc2 = x2 + 0.5+(max_clip_y-y2)*(x1-x2)/(y1-y2);	

			// test if intersection is valid, of so then done, else compute next
			if (xc2 < min_clip_x || xc2 > max_clip_x)
			{
				xc2 = min_clip_x;
				yc2 = y2 + 0.5+(min_clip_x-x2)*(y1-y2)/(x1-x2);	
			} // end if

		} break;

	default:break;

	} // end switch

	// do bounds check
	if ((xc1 < min_clip_x) || (xc1 > max_clip_x) ||
		(yc1 < min_clip_y) || (yc1 > max_clip_y) ||
		(xc2 < min_clip_x) || (xc2 > max_clip_x) ||
		(yc2 < min_clip_y) || (yc2 > max_clip_y) )
	{
		return(0);
	} // end if

	// store vars back
	x1 = xc1;
	y1 = yc1;
	x2 = xc2;
	y2 = yc2;

	return(1);

} 

void Draw_Triangle_2D(float x1,float y1, float x2,float y2, float x3,float y3, int color, UCHAR *dest_buffer, int mempitch){
	if ((FCMP(x1,x2)&&FCMP(x2,x3))||(FCMP(y1,y2)&&FCMP(y2,y3)))
	{
		return;
	}
	float tx,ty,nx;
	if (y2<y1)
	{
		SWAP(x1,x2,tx);
		SWAP(y1,y2,ty);
	}
	if(y3<y2){
		SWAP(x2,x3,tx);
		SWAP(y2,y3,ty);
	}
	if(y2<y1){
		SWAP(x1,x2,tx);
		SWAP(y1,y2,ty);
	}
	if (y3<min_clip_y||y1>max_clip_y||
		(x1<min_clip_x&&x2<min_clip_x&&x3<min_clip_x)||
		(x1>max_clip_x&&x2>max_clip_x&&x3>max_clip_x))
	{
		return;
	}
	if (FCMP(y1,y2))
	{
		Draw_Top_Tri(x3,y3,x1,y1,x2,y2,color,dest_buffer,mempitch);
		return;
	}
	if (FCMP(y2,y3))
	{
		Draw_Bottom_Tri(x1,y1,x2,y2,x3,y3,color,dest_buffer,mempitch);
		return;
	}
	nx = (y2-y1)*(x3-x1)/(y3-y1)+x1;
	Draw_Top_Tri(x3,y3,x2,y2,nx,y2,color,dest_buffer,mempitch);
	Draw_Bottom_Tri(x1,y1,x2,y2,nx,y2,color,dest_buffer,mempitch);
};

void Draw_Bottom_Tri(float x1,float y1, float x2,float y2, float x3,float y3, int color, UCHAR *dest_buffer, int mempitch){
	float tx,left,right,xs,xe;
	int iy1,iy3,loopy;
	UINT* lp_buffer = (UINT*)dest_buffer;
	UINT* lp_addr = NULL;
	mempitch = mempitch>>2;
	if (x3<x2)
	{
		SWAP(x2,x3,tx);
	}
	left = (x2-x1)/(y2-y1);
	right = (x3-x1)/(y3-y1);
	xs = x1;
	xe = x1;
	if (y1<=min_clip_y)
	{
		xs += (min_clip_y - y1)*left;
		xe += (min_clip_y - y1)*right;
		y1 = min_clip_y;
		iy1 = y1;
	} 
	else
	{
		iy1 = ceil(y1);
		xs += (iy1 - y1)*left;
		xe += (iy1 - y1)*right;
	}
	if (y3>=max_clip_y)
	{
		y3 = max_clip_y;
		iy3 = max_clip_y - 1;
	} 
	else
	{
		iy3 = ceil(y3) - 1;
	}
	lp_addr = lp_buffer + iy1*mempitch;
	if(x1>=min_clip_x&&x1<=max_clip_x&&x2>=min_clip_x&&x2<=max_clip_x&&x3>=min_clip_x&&x3<=max_clip_x){
		for (loopy = iy1;loopy<=iy3;loopy++,lp_addr+=mempitch)
		{
			Mem_Set_QUAD(lp_addr+(int)xs,color,(int)xe-(int)xs + 1);
			xs += left;
			xe += right;
		}
	}else{
		float clip_xs,clip_xe;
		
		for (loopy = iy1;loopy<=iy3;loopy++,lp_addr+=mempitch)
		{
			clip_xe = xe;
			clip_xs = xs;
			xs += left;
			xe += right;
			if (clip_xe<min_clip_x)
			{
				continue;
			}
			if (clip_xs>max_clip_x)
			{
				continue;
			}
			if (clip_xs<min_clip_x)
			{
				clip_xs = min_clip_x;
			}
			if (clip_xe>max_clip_x)
			{
				clip_xe = max_clip_x;
			}
			Mem_Set_QUAD(lp_addr+(int)clip_xs,color,(int)clip_xe-(int)clip_xs + 1);
			
		}		
	}
};

void Draw_Top_Tri(float x1,float y1, float x2,float y2, float x3,float y3, int color, UCHAR *dest_buffer, int mempitch){
	float tx,left,right,xs,xe;
	int iy1,iy3,loopy;
	UINT* lp_buffer = (UINT*)dest_buffer;
	UINT* lp_addr = NULL;
	mempitch = mempitch>>2;
	if (x3<x2)
	{
		SWAP(x2,x3,tx);
	}
	left = (x2-x1)/(y2-y1);
	right = (x3-x1)/(y3-y1);
	xs = x1;
	xe = x1;
	if (y1>=max_clip_y)
	{
		xs += (max_clip_y - y1)*left;
		xe += (max_clip_y - y1)*right;
		y1 = max_clip_y;
		iy1 = y1;
	} 
	else
	{
		iy1 = floor(y1);
		xs += (iy1 - y1)*left;
		xe += (iy1 - y1)*right;
	}
	if (y3<=min_clip_y)
	{
		y3 = min_clip_y;
		iy3 = min_clip_y;
	} 
	else
	{
		iy3 = ceil(y3);
	}
	lp_addr = lp_buffer + iy1*mempitch;
	if(x1>=min_clip_x&&x1<=max_clip_x&&x2>=min_clip_x&&x2<=max_clip_x&&x3>=min_clip_x&&x3<=max_clip_x){
		for (loopy = iy1;loopy>=iy3;loopy--,lp_addr-=mempitch)
		{
			Mem_Set_QUAD(lp_addr+(int)xs,color,(int)xe-(int)xs + 1);
			xs -= left;
			xe -= right;
		}
	}else{
		float clip_xs,clip_xe;

		for (loopy = iy1;loopy>=iy3;loopy--,lp_addr-=mempitch)
		{
			clip_xe = xe;
			clip_xs = xs;
			xs -= left;
			xe -= right;
			if (clip_xe<min_clip_x)
			{
				continue;
			}
			if (clip_xs>max_clip_x)
			{
				continue;
			}
			if (clip_xs<min_clip_x)
			{
				clip_xs = min_clip_x;
			}
			if (clip_xe>max_clip_x)
			{
				clip_xe = max_clip_x;
			}
			Mem_Set_QUAD(lp_addr+(int)clip_xs,color,(int)clip_xe-(int)clip_xs + 1);
			
		}		
	}
};

void Draw_Gouraud_Triangle(float x1,float y1, float x2,float y2, float x3,float y3, Color color0,Color color1,Color color2, UCHAR *dest_buffer, int mempitch){
	if ((FCMP(x1,x2)&&FCMP(x2,x3))||(FCMP(y1,y2)&&FCMP(y2,y3)))
	{
		return;
	}
	float tx,ty,nx;
	UINT t_color;
	Color temp_color;
	if (y2<y1)
	{
		SWAP(x1,x2,tx);
		SWAP(y1,y2,ty);
		SWAP(color0.argb,color1.argb,t_color);
	}
	if(y3<y2){
		SWAP(x2,x3,tx);
		SWAP(y2,y3,ty);
		SWAP(color1.argb,color2.argb,t_color);
	}
	if(y2<y1){
		SWAP(x1,x2,tx);
		SWAP(y1,y2,ty);
		SWAP(color0.argb,color1.argb,t_color);
	}
	if (y3<min_clip_y||y1>max_clip_y||
		(x1<min_clip_x&&x2<min_clip_x&&x3<min_clip_x)||
		(x1>max_clip_x&&x2>max_clip_x&&x3>max_clip_x))
	{
		return;
	}
	if (FCMP(y1,y2))
	{
		Draw_top_Gouraud_Triangle(x3,y3,x1,y1,x2,y2,color2,color0,color1,dest_buffer,mempitch);
		return;
	}
	if (FCMP(y2,y3))
	{
		Draw_bottom_Gouraud_Triangle(x1,y1,x2,y2,x3,y3,color0,color1,color2,dest_buffer,mempitch);
		return;
	}
	nx = (y2-y1)*(x3-x1)/(y3-y1)+x1;

	float d = (y3-y2)/(y3-y1);
	float tr = color2.r + (color0.r - color2.r)*d;
	float tg = color2.g + (color0.g - color2.g)*d;
	float tb = color2.b + (color0.b - color2.b)*d;
	temp_color.init(_RGB32BIT(0xff,(int)tr,(int)tg,(int)tb));	
	Draw_top_Gouraud_Triangle(x3,y3,x2,y2,nx,y2,color2,color1,temp_color,dest_buffer,mempitch);

	d = (y1-y2)/(y1-y3);
	tr = color0.r + (color2.r - color0.r)*d;
	tg = color0.g + (color2.g - color0.g)*d;
	tb = color0.b + (color2.b - color0.b)*d;
	temp_color.init(_RGB32BIT(0xff,(int)tr,(int)tg,(int)tb));
	Draw_bottom_Gouraud_Triangle(x1,y1,x2,y2,nx,y2,color0,color1,temp_color,dest_buffer,mempitch);
};

void Draw_bottom_Gouraud_Triangle(float x1,float y1, float x2,float y2, float x3,float y3, Color color0,Color color1,Color color2, UCHAR *dest_buffer, int mempitch){
	float tx,left,right,xs,xe;
	float dr_left,dg_left,db_left,dr_right,dg_right,db_right;
	float rs,gs,bs,rd,gd,bd;
	int iy1,iy3,loopy,loopx;
	UINT* lp_buffer = (UINT*)dest_buffer;
	UINT* lp_addr = NULL;
	UINT t_color;
	mempitch = mempitch>>2;
	if (x3<x2)
	{
		SWAP(x2,x3,tx);
		SWAP(color1.argb,color2.argb,t_color);
	}
	float y21 = 1/(y2-y1);
	float y31 = 1/(y3-y1);
	left = (x2-x1)*y21;
	right = (x3-x1)*y31;
	dr_left = (color1.r - color0.r)*y21;
	dg_left = (color1.g - color0.g)*y21;
	db_left = (color1.b - color0.b)*y21;
	dr_right = (color2.r - color0.r)*y31;
	dg_right = (color2.g - color0.g)*y31;
	db_right = (color2.b - color0.b)*y31;
	rs = color0.r;gs = color0.g;bs = color0.b;
	rd = color0.r;gd = color0.g;bd = color0.b;
	xs = x1;
	xe = x1;
	if (y1<=min_clip_y)
	{
		xs += (min_clip_y - y1)*left;
		xe += (min_clip_y - y1)*right;
		rs += (min_clip_y - y1)*dr_left;
		gs += (min_clip_y - y1)*dg_left;
		bs += (min_clip_y - y1)*db_left;
		rd += (min_clip_y - y1)*dr_right;
		gd += (min_clip_y - y1)*dg_right;
		bd += (min_clip_y - y1)*db_right;
		y1 = min_clip_y;
		iy1 = y1;
	} 
	else
	{
		iy1 = ceil(y1);
		xs += (iy1 - y1)*left;
		xe += (iy1 - y1)*right;
	}
	if (y3>=max_clip_y)
	{
		y3 = max_clip_y;
		iy3 = max_clip_y - 1;
	} 
	else
	{
		iy3 = ceil(y3) - 1;
	}
	lp_addr = lp_buffer + iy1*mempitch;
	int ixs;
	float loopx_dx,dx;
	UINT final_color;
	if(x1>=min_clip_x&&x1<=max_clip_x&&x2>=min_clip_x&&x2<=max_clip_x&&x3>=min_clip_x&&x3<=max_clip_x){
		for (loopy = iy1;loopy<=iy3;loopy++,lp_addr+=mempitch)
		{
			dx = xe - xs + 1;
			ixs = xs;
			for (loopx = 0;loopx<dx;loopx++)
			{
				loopx_dx = (loopx/dx);
				final_color = _RGB32BIT(0xff,(int)(rs + (rd-rs)*loopx_dx),(int)(gs + (gd-gs)*loopx_dx),(int)(bs + (bd-bs)*loopx_dx));
				Mem_Set_QUAD(lp_addr+ixs+loopx,final_color,1);
			}
			xs += left;
			xe += right;
			rs += dr_left;
			gs += dg_left;
			bs += db_left;
			rd += dr_right;
			gd += dg_right;
			bd += db_right;
		}
	}else{
		float clip_xs,clip_xe;

		for (loopy = iy1;loopy<=iy3;loopy++,lp_addr+=mempitch)
		{
			clip_xe = xe;
			clip_xs = xs;
			xs += left;
			xe += right;
			rs += dr_left;
			gs += dg_left;
			bs += db_left;
			rd += dr_right;
			gd += dg_right;
			bd += db_right;
			if (clip_xe<min_clip_x)
			{
				continue;
			}
			if (clip_xs>max_clip_x)
			{
				continue;
			}
			if (clip_xs<min_clip_x)
			{
				clip_xs = min_clip_x;
				rs += (min_clip_x - clip_xs)*dr_left;
				gs += (min_clip_x - clip_xs)*dg_left;
				bs += (min_clip_x - clip_xs)*db_left;
			}
			if (clip_xe>max_clip_x)
			{
				clip_xe = max_clip_x;
				rd -= (clip_xe - max_clip_x)*dr_right;
				gd -= (clip_xe - max_clip_x)*dg_right;
				bd -= (clip_xe - max_clip_x)*db_right;
			}
			dx = clip_xe - clip_xs + 1;
			ixs = clip_xs;
			for (loopx = 0;loopx<dx;loopx++)
			{
				loopx_dx = (loopx/dx);
				final_color = _RGB32BIT(0xff,(int)(rs + (rd-rs)*loopx_dx),(int)(gs + (gd-gs)*loopx_dx),(int)(bs + (bd-bs)*loopx_dx));
				Mem_Set_QUAD(lp_addr+ixs+loopx,final_color,1);
			}
		}		
	}

};
void Draw_top_Gouraud_Triangle(float x1,float y1, float x2,float y2, float x3,float y3,Color color0,Color color1,Color color2, UCHAR *dest_buffer, int mempitch){
	float tx,left,right,xs,xe;
	float dr_left,dg_left,db_left,dr_right,dg_right,db_right;
	float rs,gs,bs,rd,gd,bd;
	int iy1,iy3,loopy,loopx;
	UINT* lp_buffer = (UINT*)dest_buffer;
	UINT* lp_addr = NULL;
	UINT t_color;
	mempitch = mempitch>>2;
	if (x3<x2)
	{
		SWAP(x2,x3,tx);
		SWAP(color1.argb,color2.argb,t_color);
	}
	//这两个数其实一样的。。。y2 == y3
	float y21 = 1/(y2-y1);
	float y31 = 1/(y3-y1);
	left = (x2-x1)*y21;
	right = (x3-x1)*y31;
	xs = x1;
	xe = x1;
	dr_left = (color0.r - color1.r)*y21;
	dg_left = (color0.g - color1.g)*y21;
	db_left = (color0.b - color1.b)*y21;
	dr_right = (color0.r - color2.r)*y31;
	dg_right = (color0.g - color2.g)*y31;
	db_right = (color0.b - color2.b)*y31;
	rs = color0.r;gs = color0.g;bs = color0.b;
	rd = color0.r;gd = color0.g;bd = color0.b;
	if (y1>=max_clip_y)
	{
		xs += (max_clip_y - y1)*left;
		xe += (max_clip_y - y1)*right;
		rs -= (max_clip_y - y1)*dr_left;
		gs -= (max_clip_y - y1)*dg_left;
		bs -= (max_clip_y - y1)*db_left;
		rd -= (max_clip_y - y1)*dr_right;
		gd -= (max_clip_y - y1)*dg_right;
		bd -= (max_clip_y - y1)*db_right;
		y1 = max_clip_y;
		iy1 = y1;
	} 
	else
	{
		iy1 = floor(y1);
		xs += (iy1 - y1)*left;
		xe += (iy1 - y1)*right;
	}
	if (y3<=min_clip_y)
	{
		y3 = min_clip_y;
		iy3 = min_clip_y;
	} 
	else
	{
		iy3 = ceil(y3);
	}
	lp_addr = lp_buffer + iy1*mempitch;
	int ixs;
	float loopx_dx,dx;
	UINT final_color;
	if(x1>=min_clip_x&&x1<=max_clip_x&&x2>=min_clip_x&&x2<=max_clip_x&&x3>=min_clip_x&&x3<=max_clip_x){
		for (loopy = iy1;loopy>=iy3;loopy--,lp_addr-=mempitch)
		{
			dx = xe - xs + 1;
			ixs = xs;
			//Mem_Set_QUAD(lp_addr+(int)xs,color,(int)xe-(int)xs + 1);			
			for (loopx = 0;loopx<dx;loopx++)
			{
				loopx_dx = (loopx/dx);
				final_color = _RGB32BIT(0xff,(int)(rs + (rd-rs)*loopx_dx),(int)(gs + (gd-gs)*loopx_dx),(int)(bs + (bd-bs)*loopx_dx));
				Mem_Set_QUAD(lp_addr+ixs+loopx,final_color,1);
			}
			xs -= left;
			xe -= right;
			rs += dr_left;
			gs += dg_left;
			bs += db_left;
			rd += dr_right;
			gd += dg_right;
			bd += db_right;
		}
	}else{
		float clip_xs,clip_xe;

		for (loopy = iy1;loopy>=iy3;loopy--,lp_addr-=mempitch)
		{
			clip_xe = xe;
			clip_xs = xs;
			xs -= left;
			xe -= right;
			rs += dr_left;
			gs += dg_left;
			bs += db_left;
			rd += dr_right;
			gd += dg_right;
			bd += db_right;
			if (clip_xe<min_clip_x)
			{
				continue;
			}
			if (clip_xs>max_clip_x)
			{
				continue;
			}
			if (clip_xs<min_clip_x)
			{
				rs += (min_clip_x - clip_xs)*dr_left;
				gs += (min_clip_x - clip_xs)*dg_left;
				bs += (min_clip_x - clip_xs)*db_left;
				clip_xs = min_clip_x;
				
			}
			if (clip_xe>max_clip_x)
			{
				rd -= (clip_xe - max_clip_x)*dr_right;
				gd -= (clip_xe - max_clip_x)*dg_right;
				bd -= (clip_xe - max_clip_x)*db_right;
				clip_xe = max_clip_x;				
			}
			dx = clip_xe - clip_xs + 1;
			ixs = clip_xs;
			//Mem_Set_QUAD(lp_addr+(int)clip_xs,color,(int)clip_xe-(int)clip_xs + 1);
			for (loopx = 0;loopx<dx;loopx++)
			{
				loopx_dx = (loopx/dx);
				final_color = _RGB32BIT(0xff,(int)(rs + (rd-rs)*loopx_dx),(int)(gs + (gd-gs)*loopx_dx),(int)(bs + (bd-bs)*loopx_dx));
				Mem_Set_QUAD(lp_addr+ixs+loopx,final_color,1);
			}
		}		
	}
};

void drawTextureTriangle(RenderPoly *poly,BitmapData *texture,UCHAR *dest_buffer,int mempitch){
	Vertex3d data1;
	Vertex3d data2;
	Vertex3d data3;
	Vertex3d *v1;
	Vertex3d *v2;
	Vertex3d *v3;
	data1.pos = poly->tvlist[0].pos;
	data2.pos = poly->tvlist[1].pos;
	data3.pos = poly->tvlist[2].pos;
	data1.texture_pos =  poly->tvlist[0].texture_pos;
	data2.texture_pos =  poly->tvlist[1].texture_pos;
	data3.texture_pos =  poly->tvlist[2].texture_pos;
	v1 = &data1;
	v2 = &data2;
	v3 = &data3;
	Vertex3d *tv;
	if(v2->y<v1->y){
		SWAP(v1,v2,tv); 
	}
	if (v3->y<v2->y)
	{
		SWAP(v2,v3,tv);
	}
	if(v2->y<v1->y){
		SWAP(v1,v2,tv);
	}

	if (FCMP(v2->y,v3->y))
	{
		drawBottomTextureTriangle(v1,v2,v3,texture,dest_buffer,mempitch,poly->lit_color[0]);
		return;
	}
	if (FCMP(v1->y,v2->y))
	{
		drawTopTextureTriangle(v3,v1,v2,texture,dest_buffer,mempitch,poly->lit_color[0]);
		return;
	}
	Vertex3d temp1;
	Vertex3d temp2;
	temp1.x = (v2->y-v1->y)*(v3->x-v1->x)/(v3->y-v1->y)+v1->x;
	temp1.y = v2->y;
	temp1.ty = (v2->y-v1->y)*(v3->ty-v1->ty)/(v3->y-v1->y)+v1->ty;
	temp1.tx = (v2->y-v1->y)*(v3->tx-v1->tx)/(v3->y-v1->y)+v1->tx;
	temp2 = temp1;
	drawBottomTextureTriangle(v1,v2,&temp1,texture,dest_buffer,mempitch,poly->lit_color[0]);
	drawTopTextureTriangle(v3,&temp2,v2,texture,dest_buffer,mempitch,poly->lit_color[0]);
};

void drawBottomTextureTriangle(Vertex3d *v1,Vertex3d *v2,Vertex3d *v3,BitmapData *texture,UCHAR *dest_buffer, int mempitch,Color color){
	Vertex3d *tv;
	if (v3->x<v2->x)
	{
		SWAP(v2,v3,tv);
	}
	float x1 = v1->x;
	float x2 = v2->x;
	float x3 = v3->x;
	float y1 = v1->y;
	float y2 = v2->y;
	float y3 = v3->y;
	float dtx_left,dtx_right,dty_left,dty_right;
	float txs,txe,tys,tye;
	float left,right,xs,xe;
	int iy1,iy3,loopy,loopx;
	UINT* lp_buffer = (UINT*)dest_buffer;
	UINT* lp_addr = NULL;
	mempitch = mempitch>>2;
	float y21 = 1/(y2-y1);
	left = (x2-x1)*y21;
	right = (x3-x1)*y21;
	dtx_left = (v2->tx - v1->tx)*y21;
	dtx_right = (v3->tx - v1->tx)*y21;
	dty_left = (v2->ty - v1->ty)*y21;
	dty_right = (v3->ty - v1->ty)*y21;
	xs = x1;
	xe = x1;
	txs = v1->tx;
	txe = v1->tx;
	tys = v1->ty;
	tye = v1->ty;
	if (y1<=min_clip_y)
	{
		xs += (min_clip_y - y1)*left;
		xe += (min_clip_y - y1)*right;
		txs += (min_clip_y - y1)*dtx_left;
		txe += (min_clip_y - y1)*dtx_right;
		tys += (min_clip_y - y1)*dty_left;
		tye += (min_clip_y - y1)*dty_right;
		y1 = min_clip_y;
		iy1 = y1;
	} 
	else
	{
		iy1 = ceil(y1);
		xs += (iy1 - y1)*left;
		xe += (iy1 - y1)*right;
		txs += (iy1 - y1)*dtx_left;
		txe += (iy1 - y1)*dtx_right;
		tys += (iy1 - y1)*dty_left;
		tye += (iy1 - y1)*dty_right;
	}
	if (y3>=max_clip_y)
	{
		y3 = max_clip_y;
		iy3 = max_clip_y - 1;
	} 
	else
	{
		iy3 = ceil(y3) - 1;
	}
	lp_addr = lp_buffer + iy1*mempitch;
	int ixs;
	int texture_x;
	int texture_y;
	float dx;
	float tx_step;
	float ty_step;
	UINT final_color;

	float light_r = (float)color.r/256;
	float light_g = (float)color.g/256;
	float light_b = (float)color.b/256;
	Color temp_color;
	if(x1>=min_clip_x&&x1<=max_clip_x&&x2>=min_clip_x&&x2<=max_clip_x&&x3>=min_clip_x&&x3<=max_clip_x){
		for (loopy = iy1;loopy<=iy3;loopy++,lp_addr+=mempitch)
		{
			dx = xe - xs + 1;
			tx_step = (txe - txs)/dx;
			ty_step = (tye - tys)/dx;
			ixs = xs;
			for (loopx = 0;loopx<(int)dx;loopx++)
			{
				texture_x = (txs + loopx*tx_step)*(texture->width - 1);
				texture_y = (tys + loopx*ty_step)*(texture->height - 1);
				temp_color = texture->data[(texture_y<<texture->power_of_two)+texture_x];
				temp_color.r *= light_r;
				temp_color.g *= light_g;
				temp_color.b *= light_b;
				final_color = temp_color.argb;
				Mem_Set_QUAD(lp_addr+ixs+loopx,final_color,1);
			}
			xs += left;
			xe += right;
			txs += dtx_left;
			txe += dtx_right;
			tys += dty_left;
			tye += dty_right;
		}
	}else{
		float clip_xs,clip_xe;
		float clip_txs,clip_tys;
		for (loopy = iy1;loopy<=iy3;loopy++,lp_addr+=mempitch)
		{
			clip_xe = xe;
			clip_xs = xs;
			clip_txs = txs;
			clip_tys = tys;
			tx_step = (txe - txs)/(clip_xe - clip_xs + 1);
			ty_step = (tye - tys)/(clip_xe - clip_xs + 1);
			//下面这段必须放这里，不能放后面，因为下面continue后这个值也必须变化
			xs += left;
			xe += right;
			txs += dtx_left;
			txe += dtx_right;
			tys += dty_left;
			tye += dty_right;
			////////////////// 
			
			if (clip_xe<min_clip_x)
			{
				continue;
			}
			if (clip_xs>max_clip_x)
			{
				continue;
			}
			if (clip_xs<min_clip_x)
			{				
				clip_txs += (min_clip_x - clip_xs)*tx_step;
				clip_tys += (min_clip_x - clip_xs)*ty_step;
				clip_xs = min_clip_x;
			}
			if (clip_xe>max_clip_x)
			{				
				clip_xe = max_clip_x;
			}
			dx = clip_xe - clip_xs + 1;
			ixs = clip_xs;
			
			for (loopx = 0;loopx<(int)dx;loopx++)
			{
				texture_x = (clip_txs + loopx*tx_step)*(texture->width - 1);
				texture_y = (clip_tys + loopx*ty_step)*(texture->height - 1);
				temp_color = texture->data[(texture_y<<texture->power_of_two)+texture_x];
				temp_color.r *= light_r;
				temp_color.g *= light_g;
				temp_color.b *= light_b;
				final_color = temp_color.argb;
				Mem_Set_QUAD(lp_addr+ixs+loopx,final_color,1);
			}
			
			
		}		
	}
};

void drawTopTextureTriangle(Vertex3d *v1,Vertex3d *v2,Vertex3d *v3,BitmapData *texture,UCHAR *dest_buffer, int mempitch,Color color){
	Vertex3d *tv;
	if (v3->x<v2->x)
	{
		SWAP(v2,v3,tv);
	}
	float x1 = v1->x;
	float x2 = v2->x;
	float x3 = v3->x;
	float y1 = v1->y;
	float y2 = v2->y;
	float y3 = v3->y;
	float dtx_left,dtx_right,dty_left,dty_right;
	float txs,txe,tys,tye;
	float left,right,xs,xe;
	int iy1,iy3,loopy,loopx;
	UINT* lp_buffer = (UINT*)dest_buffer;
	UINT* lp_addr = NULL;
	mempitch = mempitch>>2;
	float y21 = 1/(y2-y1);
	left = (x2-x1)*y21;
	right = (x3-x1)*y21;
	xs = x1;
	xe = x1;

	dtx_left = -(v2->tx - v1->tx)*y21;
	dtx_right = -(v3->tx - v1->tx)*y21;
	dty_left = -(v2->ty - v1->ty)*y21;
	dty_right = -(v3->ty - v1->ty)*y21;
	txs = v1->tx;
	txe = v1->tx;
	tys = v1->ty;
	tye = v1->ty;

	if (y1>=max_clip_y)
	{
		xs += (max_clip_y - y1)*left;
		xe += (max_clip_y - y1)*right;
		txs -= (max_clip_y - y1)*dtx_left;
		txe -= (max_clip_y - y1)*dtx_right;
		tys -= (max_clip_y - y1)*dty_left;
		tye -= (max_clip_y - y1)*dty_right;
		y1 = max_clip_y;
		iy1 = y1;
	} 
	else
	{
		iy1 = floor(y1);
		xs += (iy1 - y1)*left;
		xe += (iy1 - y1)*right;
		txs -= (iy1 - y1)*dtx_left;
		txe -= (iy1 - y1)*dtx_right;
		tys -= (iy1 - y1)*dty_left;
		tye -= (iy1 - y1)*dty_right;
	}
	if (y3<=min_clip_y)
	{
		y3 = min_clip_y;
		iy3 = min_clip_y;
	} 
	else
	{
		iy3 = ceil(y3);
	}
	lp_addr = lp_buffer + iy1*mempitch;
	int ixs;
	int texture_x;
	int texture_y;
	float loopx_dx,dx;
	float tx_step;
	float ty_step;
	UINT final_color;

	float light_r = (float)color.r/256;
	float light_g = (float)color.g/256;
	float light_b = (float)color.b/256;
	Color temp_color;
	if(x1>=min_clip_x&&x1<=max_clip_x&&x2>=min_clip_x&&x2<=max_clip_x&&x3>=min_clip_x&&x3<=max_clip_x){
		for (loopy = iy1;loopy>=iy3;loopy--,lp_addr-=mempitch)
		{
			dx = xe - xs + 1;
			tx_step = (txe - txs)/dx;
			ty_step = (tye - tys)/dx;
			ixs = xs;
			for (loopx = 0;loopx<(int)dx;loopx++)
			{
				texture_x = (txs + loopx*tx_step)*(texture->width - 1);
				texture_y = (tys + loopx*ty_step)*(texture->height - 1);
				temp_color = texture->data[(texture_y<<texture->power_of_two)+texture_x];
				temp_color.r *= light_r;
				temp_color.g *= light_g;
				temp_color.b *= light_b;
				final_color = temp_color.argb;
				Mem_Set_QUAD(lp_addr+ixs+loopx,final_color,1);
			}
			xs -= left;
			xe -= right;
			txs += dtx_left;
			txe += dtx_right;
			tys += dty_left;
			tye += dty_right;
		}
	}else{
		float clip_xs,clip_xe;
		float clip_txs,clip_tys;
		for (loopy = iy1;loopy>=iy3;loopy--,lp_addr-=mempitch)
		{
			clip_xe = xe;
			clip_xs = xs;
			clip_txs = txs;
			clip_tys = tys;
			tx_step = (txe - txs)/(clip_xe - clip_xs);
			ty_step = (tye - tys)/(clip_xe - clip_xs);
			//下面这段必须放这里，不能放后面，因为下面continue后这个值也必须变化
			xs -= left;
			xe -= right;
			txs += dtx_left;
			txe += dtx_right;
			tys += dty_left;
			tye += dty_right;
			////////
			
			if (clip_xe<min_clip_x)
			{
				continue;
			}
			if (clip_xs>max_clip_x)
			{
				continue;
			}
			if (clip_xs<min_clip_x)
			{
				clip_txs += (min_clip_x - clip_xs)*tx_step;
				clip_tys += (min_clip_x - clip_xs)*ty_step;
				clip_xs = min_clip_x;

			}
			if (clip_xe>max_clip_x)
			{
				clip_xe = max_clip_x;				
			}
			dx = clip_xe - clip_xs + 1;
			ixs = clip_xs;
			for (loopx = 0;loopx<(int)dx;loopx++)
			{
				texture_x = (clip_txs + loopx*tx_step)*(texture->width - 1);
				texture_y = (clip_tys + loopx*ty_step)*(texture->height - 1);
				temp_color = texture->data[(texture_y<<texture->power_of_two)+texture_x];
				temp_color.r *= light_r;
				temp_color.g *= light_g;
				temp_color.b *= light_b;
				final_color = temp_color.argb;
				Mem_Set_QUAD(lp_addr+ixs+loopx,final_color,1);
			}
			
		}		
	}
};

void clipPolyFromRenderlist(RenderList* rend_list,Camera* cam,int clip_flags){
	int vertex_ccodes[3]; // used to store clipping flags
	int num_verts_in;     // number of vertices inside
	int v0, v1, v2;       // vertex indices

	float z_factor,       // used in clipping computations
		z_test;         // used in clipping computations

	float xi, yi, x01i, y01i, x02i, y02i, // vertex intersection points
		t1, t2,                         // parametric t values
		ui, vi, u01i, v01i, u02i, v02i; // texture intersection points

	int last_poly_index,            // last valid polygon in polylist
		insert_poly_index;          // the current position new polygons are inserted at

	Vector3d u,v,n;                 // used in vector calculations

	RenderPoly temp_poly;            // used when we need to split a poly into 2 polys

	insert_poly_index = last_poly_index = rend_list->num_polys;

	// traverse polygon list and clip/cull polygons
	for (int poly = 0; poly < last_poly_index; poly++)
	{
		// acquire current polygon
		RenderPoly* curr_poly = rend_list->lp_polys[poly];
		if ((curr_poly==NULL) || !(curr_poly->state & POLY4D_STATE_ACTIVE) ||
			(curr_poly->state & POLY4D_STATE_CLIPPED ) || 
			(curr_poly->state & POLY4D_STATE_BACKFACE) )
			continue; // move onto next poly   
		if (clip_flags & CLIP_POLY_X_PLANE)
		{
			z_factor = (0.5)*cam->viewplane_width/cam->view_dist;  

			// vertex 0

			z_test = z_factor*curr_poly->tvlist[0].z;

			if (curr_poly->tvlist[0].x > z_test)
				vertex_ccodes[0] = CLIP_CODE_GX;
			else
				if (curr_poly->tvlist[0].x < -z_test)
					vertex_ccodes[0] = CLIP_CODE_LX;
				else
					vertex_ccodes[0] = CLIP_CODE_IX;

			// vertex 1

			z_test = z_factor*curr_poly->tvlist[1].z;         

			if (curr_poly->tvlist[1].x > z_test)
				vertex_ccodes[1] = CLIP_CODE_GX;
			else
				if (curr_poly->tvlist[1].x < -z_test)
					vertex_ccodes[1] = CLIP_CODE_LX;
				else
					vertex_ccodes[1] = CLIP_CODE_IX;

			// vertex 2

			z_test = z_factor*curr_poly->tvlist[2].z;              

			if (curr_poly->tvlist[2].x > z_test)
				vertex_ccodes[2] = CLIP_CODE_GX;
			else
				if (curr_poly->tvlist[2].x < -z_test)
					vertex_ccodes[2] = CLIP_CODE_LX;
				else
					vertex_ccodes[2] = CLIP_CODE_IX;

			// test for trivial rejections, polygon completely beyond right or left
			// clipping planes
			if ( ((vertex_ccodes[0] == CLIP_CODE_GX) && 
				(vertex_ccodes[1] == CLIP_CODE_GX) && 
				(vertex_ccodes[2] == CLIP_CODE_GX) ) ||

				((vertex_ccodes[0] == CLIP_CODE_LX) && 
				(vertex_ccodes[1] == CLIP_CODE_LX) && 
				(vertex_ccodes[2] == CLIP_CODE_LX) ) )

			{
				curr_poly->state |= POLY4D_STATE_CLIPPED;

				// move on to next polygon
				continue;
			} // end if

		} // end if x planes

		// clip/cull to y-planes       
		if (clip_flags & CLIP_POLY_Y_PLANE)
		{
			// clip/cull only based on y clipping planes
			// for each vertice determine if it's in the clipping region or beyond it and
			// set the appropriate clipping code
			// we do NOT clip the final triangles, we are only trying to trivally reject them 
			// we are going to clip polygons in the rasterizer to the screen rectangle
			// but we do want to clip/cull polys that are totally outside the viewfrustrum

			// since we are clipping to the top/bottom y-planes we need to use the FOV or
			// the plane equations to find the z value that at the current y position would
			// be outside the plane
			z_factor = (0.5)*cam->viewplane_width/cam->view_dist;  

			// vertex 0
			z_test = z_factor*curr_poly->tvlist[0].z;

			if (curr_poly->tvlist[0].y > z_test)
				vertex_ccodes[0] = CLIP_CODE_GY;
			else
				if (curr_poly->tvlist[0].y < -z_test)
					vertex_ccodes[0] = CLIP_CODE_LY;
				else
					vertex_ccodes[0] = CLIP_CODE_IY;

			// vertex 1
			z_test = z_factor*curr_poly->tvlist[1].z;         

			if (curr_poly->tvlist[1].y > z_test)
				vertex_ccodes[1] = CLIP_CODE_GY;
			else
				if (curr_poly->tvlist[1].y < -z_test)
					vertex_ccodes[1] = CLIP_CODE_LY;
				else
					vertex_ccodes[1] = CLIP_CODE_IY;

			// vertex 2
			z_test = z_factor*curr_poly->tvlist[2].z;              

			if (curr_poly->tvlist[2].y > z_test)
				vertex_ccodes[2] = CLIP_CODE_GY;
			else
				if (curr_poly->tvlist[2].x < -z_test)
					vertex_ccodes[2] = CLIP_CODE_LY;
				else
					vertex_ccodes[2] = CLIP_CODE_IY;

			// test for trivial rejections, polygon completely beyond top or bottom
			// clipping planes
			if ( ((vertex_ccodes[0] == CLIP_CODE_GY) && 
				(vertex_ccodes[1] == CLIP_CODE_GY) && 
				(vertex_ccodes[2] == CLIP_CODE_GY) ) ||

				((vertex_ccodes[0] == CLIP_CODE_LY) && 
				(vertex_ccodes[1] == CLIP_CODE_LY) && 
				(vertex_ccodes[2] == CLIP_CODE_LY) ) )

			{
				curr_poly->state |= POLY4D_STATE_CLIPPED;
				continue;
			} // end if

		} // end if y planes

		// clip/cull to z planes
		if (clip_flags & CLIP_POLY_Z_PLANE)
		{
			// clip/cull only based on z clipping planes
			// for each vertice determine if it's in the clipping region or beyond it and
			// set the appropriate clipping code
			// then actually clip all polygons to the near clipping plane, this will result
			// in at most 1 additional triangle

			// reset vertex counters, these help in classification
			// of the final triangle 
			num_verts_in = 0;

			// vertex 0
			if (curr_poly->tvlist[0].z > cam->far_clip_z)
			{
				vertex_ccodes[0] = CLIP_CODE_GZ;
			} 
			else
				if (curr_poly->tvlist[0].z < cam->near_clip_z)
				{
					vertex_ccodes[0] = CLIP_CODE_LZ;
				}
				else
				{
					vertex_ccodes[0] = CLIP_CODE_IZ;
					num_verts_in++;
				} 

				// vertex 1
				if (curr_poly->tvlist[1].z > cam->far_clip_z)
				{
					vertex_ccodes[1] = CLIP_CODE_GZ;
				} 
				else
					if (curr_poly->tvlist[1].z < cam->near_clip_z)
					{
						vertex_ccodes[1] = CLIP_CODE_LZ;
					}
					else
					{
						vertex_ccodes[1] = CLIP_CODE_IZ;
						num_verts_in++;
					}     

					// vertex 2
					if (curr_poly->tvlist[2].z > cam->far_clip_z)
					{
						vertex_ccodes[2] = CLIP_CODE_GZ;
					} 
					else
						if (curr_poly->tvlist[2].z < cam->near_clip_z)
						{
							vertex_ccodes[2] = CLIP_CODE_LZ;
						}
						else
						{
							vertex_ccodes[2] = CLIP_CODE_IZ;
							num_verts_in++;
						} 

						// test for trivial rejections, polygon completely beyond far or near
						// z clipping planes
						if ( ((vertex_ccodes[0] == CLIP_CODE_GZ) && 
							(vertex_ccodes[1] == CLIP_CODE_GZ) && 
							(vertex_ccodes[2] == CLIP_CODE_GZ) ) ||

							((vertex_ccodes[0] == CLIP_CODE_LZ) && 
							(vertex_ccodes[1] == CLIP_CODE_LZ) && 
							(vertex_ccodes[2] == CLIP_CODE_LZ) ) )

						{
							// clip the poly completely out of frustrum
							curr_poly->state |= POLY4D_STATE_CLIPPED;

							// move on to next polygon
							continue;
						} // end if

						// test if any vertex has protruded beyond near clipping plane?
						if ( ( (vertex_ccodes[0] | vertex_ccodes[1] | vertex_ccodes[2]) & CLIP_CODE_LZ) )
						{
							if (num_verts_in == 1)
							{
								if ( vertex_ccodes[0] == CLIP_CODE_IZ)
								{ v0 = 0; v1 = 1; v2 = 2; }
								else 
									if (vertex_ccodes[1] == CLIP_CODE_IZ)
									{ v0 = 1; v1 = 2; v2 = 0; }
									else
									{ v0 = 2; v1 = 0; v2 = 1; }

									curr_poly->tvlist[v0].pos.build(&curr_poly->tvlist[v1].pos, &v);                          

									// the intersection occurs when z = near z, so t = 
									t1 = ( (cam->near_clip_z - curr_poly->tvlist[v0].z) / v.z );

									// now plug t back in and find x,y intersection with the plane
									xi = curr_poly->tvlist[v0].x + v.x * t1;
									yi = curr_poly->tvlist[v0].y + v.y * t1;

									// now overwrite vertex with new vertex
									curr_poly->tvlist[v1].x = xi;
									curr_poly->tvlist[v1].y = yi;
									curr_poly->tvlist[v1].z = cam->near_clip_z; 

									// clip edge v0->v2
									curr_poly->tvlist[v0].pos.build(&curr_poly->tvlist[v2].pos, &v);                          

									// the intersection occurs when z = near z, so t = 
									t2 = ( (cam->near_clip_z - curr_poly->tvlist[v0].z) / v.z );

									// now plug t back in and find x,y intersection with the plane
									xi = curr_poly->tvlist[v0].x + v.x * t2;
									yi = curr_poly->tvlist[v0].y + v.y * t2;

									// now overwrite vertex with new vertex
									curr_poly->tvlist[v2].x = xi;
									curr_poly->tvlist[v2].y = yi;
									curr_poly->tvlist[v2].z = cam->near_clip_z; 

									// now that we have both t1, t2, check if the poly is textured, if so clip
									// texture coordinates
									if (curr_poly->attr & POLY4D_ATTR_SHADE_MODE_TEXTURE)
									{
										ui = curr_poly->tvlist[v0].tx + (curr_poly->tvlist[v1].tx - curr_poly->tvlist[v0].tx)*t1;
										vi = curr_poly->tvlist[v0].ty + (curr_poly->tvlist[v1].ty - curr_poly->tvlist[v0].ty)*t1;
										curr_poly->tvlist[v1].tx = ui;
										curr_poly->tvlist[v1].ty = vi;

										ui = curr_poly->tvlist[v0].tx + (curr_poly->tvlist[v2].tx - curr_poly->tvlist[v0].tx)*t2;
										vi = curr_poly->tvlist[v0].ty + (curr_poly->tvlist[v2].ty - curr_poly->tvlist[v0].ty)*t2;
										curr_poly->tvlist[v2].tx = ui;
										curr_poly->tvlist[v2].ty = vi;
									} // end if textured

									// finally, we have obliterated our pre-computed normal length
									// it needs to be recomputed!!!!

									// build u, v
									curr_poly->tvlist[v0].pos.build(&curr_poly->tvlist[v1].pos, &u);
									curr_poly->tvlist[v0].pos.build(&curr_poly->tvlist[v2].pos, &v);

									// compute cross product
									u.cross(&v, &n);

									// compute length of normal accurately and store in poly nlength
									// +- epsilon later to fix over/underflows
									curr_poly->normal_length = vector3dLengthFast(&n); 

							} // end if
							else
								if (num_verts_in == 2)
								{ // num_verts = 2

									// must be the case with num_verts_in = 2 
									// we need to clip the triangle against the near clipping plane
									// the clipping procedure is done to each edge leading away from
									// the interior vertex, to clip we need to compute the intersection
									// with the near z plane, this is done with a parametric equation of 
									// the edge, however unlike case 1 above, the triangle will be split
									// into two triangles, thus during the first clip, we will store the 
									// results into a new triangle at the end of the rendering list, and 
									// then on the last clip we will overwrite the triangle being clipped

									// step 0: copy the polygon
									memcpy(&temp_poly, curr_poly, sizeof(RenderPoly) );

									// step 1: find vertex index for exterior vertex
									if ( vertex_ccodes[0] == CLIP_CODE_LZ)
									{ v0 = 0; v1 = 1; v2 = 2; }
									else 
										if (vertex_ccodes[1] == CLIP_CODE_LZ)
										{ v0 = 1; v1 = 2; v2 = 0; }
										else
										{ v0 = 2; v1 = 0; v2 = 1; }

										curr_poly->tvlist[v0].pos.build(&curr_poly->tvlist[v1].pos, &v);                          

										// the intersection occurs when z = near z, so t = 
										t1 = ( (cam->near_clip_z - curr_poly->tvlist[v0].z) / v.z );

										// now plug t back in and find x,y intersection with the plane
										x01i = curr_poly->tvlist[v0].x + v.x * t1;
										y01i = curr_poly->tvlist[v0].y + v.y * t1;

										// clip edge v0->v2
										curr_poly->tvlist[v0].pos.build(&curr_poly->tvlist[v2].pos, &v);                          

										// the intersection occurs when z = near z, so t = 
										t2 = ( (cam->near_clip_z - curr_poly->tvlist[v0].z) / v.z );

										// now plug t back in and find x,y intersection with the plane
										x02i = curr_poly->tvlist[v0].x + v.x * t2;
										y02i = curr_poly->tvlist[v0].y + v.y * t2; 

										// now we have both intersection points, we must overwrite the inplace
										// polygon's vertex 0 with the intersection point, this poly 1 of 2 from
										// the split

										// now overwrite vertex with new vertex
										curr_poly->tvlist[v0].x = x01i;
										curr_poly->tvlist[v0].y = y01i;
										curr_poly->tvlist[v0].z = cam->near_clip_z; 

										// now comes the hard part, we have to carefully create a new polygon
										// from the 2 intersection points and v2, this polygon will be inserted
										// at the end of the rendering list, but for now, we are building it up
										// in  temp_poly

										// so leave v2 alone, but overwrite v1 with v01, and overwrite v0 with v02
										temp_poly.tvlist[v1].x = x01i;
										temp_poly.tvlist[v1].y = y01i;
										temp_poly.tvlist[v1].z = cam->near_clip_z;              

										temp_poly.tvlist[v0].x = x02i;
										temp_poly.tvlist[v0].y = y02i;
										temp_poly.tvlist[v0].z = cam->near_clip_z;    

										// now that we have both t1, t2, check if the poly is textured, if so clip
										// texture coordinates
										if (curr_poly->attr & POLY4D_ATTR_SHADE_MODE_TEXTURE)
										{
											// compute poly 1 new texture coordinates from split
											u01i = curr_poly->tvlist[v0].tx + (curr_poly->tvlist[v1].tx - curr_poly->tvlist[v0].tx)*t1;
											v01i = curr_poly->tvlist[v0].ty + (curr_poly->tvlist[v1].ty - curr_poly->tvlist[v0].ty)*t1;

											// compute poly 2 new texture coordinates from split
											u02i = curr_poly->tvlist[v0].tx + (curr_poly->tvlist[v2].tx - curr_poly->tvlist[v0].tx)*t2;
											v02i = curr_poly->tvlist[v0].ty + (curr_poly->tvlist[v2].ty - curr_poly->tvlist[v0].ty)*t2;

											// write them all at the same time         
											// poly 1
											curr_poly->tvlist[v0].tx = u01i;
											curr_poly->tvlist[v0].ty = v01i;

											// poly 2
											temp_poly.tvlist[v0].tx = u02i;
											temp_poly.tvlist[v0].ty = v02i;
											temp_poly.tvlist[v1].tx = u01i;
											temp_poly.tvlist[v1].ty = v01i;

										} // end if textured


										// finally, we have obliterated our pre-computed normal lengths
										// they need to be recomputed!!!!

										// poly 1 first, in place

										// build u, v
										curr_poly->tvlist[v0].pos.build(&curr_poly->tvlist[v1].pos, &u);
										curr_poly->tvlist[v0].pos.build(&curr_poly->tvlist[v2].pos, &v);

										// compute cross product
										u.cross(&v, &n);

										// compute length of normal accurately and store in poly nlength
										// +- epsilon later to fix over/underflows
										curr_poly->normal_length = vector3dLengthFast(&n); 

										// now poly 2, temp_poly
										// build u, v
										temp_poly.tvlist[v0].pos.build(&temp_poly.tvlist[v1].pos, &u);
										temp_poly.tvlist[v0].pos.build(&temp_poly.tvlist[v2].pos, &v);

										// compute cross product
										u.cross(&v, &n);

										// compute length of normal accurately and store in poly nlength
										// +- epsilon later to fix over/underflows
										temp_poly.normal_length = vector3dLengthFast(&n); 

										// now we are good to go, insert the polygon into list
										// if the poly won't fit, it won't matter, the function will
										// just return 0
										rend_list->addPoly(&temp_poly);
										//Insert_POLYF4DV2_RENDERLIST4DV2(rend_list, &temp_poly);

								} // end else

						} // end if near_z clipping has occured

		} // end if z planes

	} // end for poly
};


void Draw_Triangle_zb(RenderPoly* poly,UCHAR *dest_buffer, int mempitch,float* z_buffer,int z_pitch){
	Vertex3d data1;
	Vertex3d data2;
	Vertex3d data3;
	Vertex3d *v1;
	Vertex3d *v2;
	Vertex3d *v3;
	float x1,y1,x2,y2,x3,y3,temp_value;
	float z1,z2,z3;
	mempitch>>=2;
	UINT* lp_buffer = (UINT*)dest_buffer;
	UINT* lp_addr = NULL;
	float* lp_zbuffer = z_buffer;
	float* lp_zaddr = NULL;
	int tri_type = TRI_TYPE_GENERAL;
	data1.pos = poly->tvlist[0].pos;
	data2.pos = poly->tvlist[1].pos;
	data3.pos = poly->tvlist[2].pos;
	data1.texture_pos =  poly->tvlist[0].texture_pos;
	data2.texture_pos =  poly->tvlist[1].texture_pos;
	data3.texture_pos =  poly->tvlist[2].texture_pos;
	v1 = &data1;
	v2 = &data2;
	v3 = &data3;
	Vertex3d *tv;
	if(v2->y<v1->y){
		SWAP(v1,v2,tv); 
	}
	if (v3->y<v2->y)
	{
		SWAP(v2,v3,tv);
	}
	if(v2->y<v1->y){
		SWAP(v1,v2,tv);
	}
	if (FCMP(v2->y,v3->y))
	{
		tri_type = TRI_TYPE_BOTTOM;
		if (v2->x > v3->x)
		{
			SWAP(v2,v3,tv);
		}
	}
	if (FCMP(v1->y,v2->y))
	{
		tri_type = TRI_TYPE_TOP;
		if (v1->x > v2->x)
		{
			SWAP(v1,v2,tv);
		}
	}
	x1 = v1->x;y1 = v1->y;z1 = v1->z;
	x2 = v2->x;y2 = v2->y;z2 = v2->z;
	x3 = v3->x;y3 = v3->y;z3 = v3->z;
	if (y3<min_clip_y||y1>max_clip_y||
		(x1<min_clip_x&&x2<min_clip_x&&x3<min_clip_x)||
		(x1>max_clip_x&&x2>max_clip_x&&x3>max_clip_x))
	{
		return;
	}
	int iy1,iy3,loopy,loopx,ixs;
	float dxdyl,dzdyl,dxdyr,dzdyr,dy,xs,xe,dzdx,zs,ze,dx,current_z;
	switch(tri_type){
	case TRI_TYPE_BOTTOM:
		dy = y2-y1;
		dxdyl = (x2-x1)/dy;
		dxdyr = (x3-x1)/dy;
		dzdyl = (z2-z1)/dy;
		dzdyr = (z3-z1)/dy;
		xs = x1;
		xe = x1;
		zs = z1;
		ze = z1;

		if (y1<=min_clip_y)
		{
			xs += (min_clip_y - y1)*dxdyl;
			xe += (min_clip_y - y1)*dxdyr;
			zs += (min_clip_y - y1)*dzdyl;
			ze += (min_clip_y - y1)*dzdyr;
			y1 = min_clip_y;
			iy1 = y1;
		}else
		{
			iy1 = ceil(y1);
			xs += (iy1 - y1)*dxdyl;
			xe += (iy1 - y1)*dxdyr;
			zs += (iy1 - y1)*dzdyl;
			ze += (iy1 - y1)*dzdyr;
		}
		if (y3>=max_clip_y)
		{
			y3 = max_clip_y;
			iy3 = max_clip_y - 1;
		} 
		else
		{
			iy3 = ceil(y3) - 1;
		}
		lp_addr = lp_buffer + ((int)y1)*mempitch;
		lp_zaddr = lp_zbuffer + ((int)y1)*z_pitch;
		if(x1>=min_clip_x&&x1<=max_clip_x&&x2>=min_clip_x&&x2<=max_clip_x&&x3>=min_clip_x&&x3<=max_clip_x){
			for (loopy = iy1;loopy<=iy3;loopy++,lp_addr+=mempitch,lp_zaddr+=z_pitch)
			{
				dx = xe-xs+1;
				dzdx = (ze-zs)/dx;
				ixs = xs;
				for (loopx = 0;loopx<(int)dx;loopx++)
				{
					current_z = zs + loopx*dzdx;
					if (current_z < *(lp_zaddr+ixs+loopx))
					{
						Mem_Set_QUAD(lp_addr+ixs+loopx,poly->lit_color[0].argb,1);
						*(lp_zaddr+ixs+loopx) = current_z;
					}
				}
				xs += dxdyl;
				xe += dxdyr;
				zs += dzdyl;
				ze += dzdyr;
			}
		}else{
			float clip_xs,clip_xe,clip_zs,clip_ze;

			for (loopy = iy1;loopy<=iy3;loopy++,lp_addr+=mempitch,lp_zaddr+=z_pitch)
			{
				clip_xe = xe;
				clip_xs = xs;
				clip_ze = ze;
				clip_zs = zs;
				xs += dxdyl;
				xe += dxdyr;
				zs += dzdyl;
				ze += dzdyr;
				if (clip_xe<min_clip_x)
				{
					continue;
				}
				if (clip_xs>max_clip_x)
				{
					continue;
				}
				if (clip_xs<min_clip_x)
				{
					clip_xs = min_clip_x;
				}
				if (clip_xe>max_clip_x)
				{
					clip_xe = max_clip_x;
				}
				dx = clip_xe-clip_xs+1;
				dzdx = (ze-zs)/dx;
				zs += (clip_xs - xs)*dzdx;
				ixs = xs;
				for (loopx = 0;loopx<(int)dx;loopx++)
				{
					current_z = zs + loopx*dzdx;
					if (current_z < *(lp_zaddr+ixs+loopx))
					{
						Mem_Set_QUAD(lp_addr+ixs+loopx,poly->lit_color[0].argb,1);
						*(lp_zaddr+ixs+loopx) = current_z;
					}
				}
			}
		}
		break;
	case TRI_TYPE_TOP:
		dxdyl = (x3-x1)/(y3-y1);
		dxdyr = (x3-x2)/(y3-y2);
		dzdyl = (z3-z1)/(y3-y1);
		dzdyr = (z3-z2)/(y3-y2);
		xs = x1;
		xe = x2;
		zs = z1;
		ze = z2;

		if (y1<=min_clip_y)
		{
			xs += (min_clip_y - y1)*dxdyl;
			xe += (min_clip_y - y1)*dxdyr;
			zs += (min_clip_y - y1)*dzdyl;
			ze += (min_clip_y - y1)*dzdyr;
			y1 = min_clip_y;
			iy1 = y1;
		}else
		{
			iy1 = ceil(y1);
			xs += (iy1 - y1)*dxdyl;
			xe += (iy1 - y1)*dxdyr;
			zs += (iy1 - y1)*dzdyl;
			ze += (iy1 - y1)*dzdyr;
		}
		if (y3>=max_clip_y)
		{
			y3 = max_clip_y;
			iy3 = max_clip_y - 1;
		} 
		else
		{
			iy3 = ceil(y3) - 1;
		}
		lp_addr = lp_buffer + ((int)y1)*mempitch;
		lp_zaddr = lp_zbuffer + ((int)y1)*z_pitch;
		if(x1>=min_clip_x&&x1<=max_clip_x&&x2>=min_clip_x&&x2<=max_clip_x&&x3>=min_clip_x&&x3<=max_clip_x){
			for (loopy = iy1;loopy<=iy3;loopy++,lp_addr+=mempitch,lp_zaddr+=z_pitch)
			{
				dx = xe-xs+1;
				dzdx = (ze-zs)/dx;
				ixs = xs;
				for (loopx = 0;loopx<(int)dx;loopx++)
				{
					current_z = zs + loopx*dzdx;
					if (current_z < *(lp_zaddr+ixs+loopx))
					{
						Mem_Set_QUAD(lp_addr+ixs+loopx,poly->lit_color[0].argb,1);
						*(lp_zaddr+ixs+loopx) = current_z;
					}
				}
				xs += dxdyl;
				xe += dxdyr;
				zs += dzdyl;
				ze += dzdyr;
			}
		}else{
			float clip_xs,clip_xe,clip_zs,clip_ze;

			for (loopy = iy1;loopy<=iy3;loopy++,lp_addr+=mempitch,lp_zaddr+=z_pitch)
			{
				clip_xe = xe;
				clip_xs = xs;
				clip_ze = ze;
				clip_zs = zs;
				xs += dxdyl;
				xe += dxdyr;
				zs += dzdyl;
				ze += dzdyr;
				if (clip_xe<min_clip_x)
				{
					continue;
				}
				if (clip_xs>max_clip_x)
				{
					continue;
				}
				if (clip_xs<min_clip_x)
				{
					clip_xs = min_clip_x;
				}
				if (clip_xe>max_clip_x)
				{
					clip_xe = max_clip_x;
				}
				dx = clip_xe-clip_xs+1;
				dzdx = (ze-zs)/dx;
				zs += (clip_xs - xs)*dzdx;
				ixs = xs;
				for (loopx = 0;loopx<(int)dx;loopx++)
				{
					current_z = zs + loopx*dzdx;
					if (current_z < *(lp_zaddr+ixs+loopx))
					{
						Mem_Set_QUAD(lp_addr+ixs+loopx,poly->lit_color[0].argb,1);
						*(lp_zaddr+ixs+loopx) = current_z;
					}
				}
			}
		}
		break;
	case TRI_TYPE_GENERAL:
		float temp[12] = {x1,y1,z1,x2,y2,z2,(y2-y1)*(x3-x1)/(y3-y1)+x1,y2,(y2-y1)*(z3-z1)/(y3-y1)+z1,x3,y3,z3};
		x1 = temp[0];y1 = temp[1];z1 = temp[2];
		x2 = temp[3];y2 = temp[4];z2 = temp[5];
		x3 = temp[6];y3 = temp[7];z3 = temp[8];

		if (x2>x3)
		{
			SWAP(x2,x3,temp_value);
			SWAP(y2,y3,temp_value);
			SWAP(z2,z3,temp_value);
		}

		
		dy = y2-y1;
		dxdyl = (x2-x1)/dy;
		dxdyr = (x3-x1)/dy;
		dzdyl = (z2-z1)/dy;
		dzdyr = (z3-z1)/dy;
		xs = x1;
		xe = x1;
		zs = z1;
		ze = z1;

		if (y1<=min_clip_y)
		{
			xs += (min_clip_y - y1)*dxdyl;
			xe += (min_clip_y - y1)*dxdyr;
			zs += (min_clip_y - y1)*dzdyl;
			ze += (min_clip_y - y1)*dzdyr;
			y1 = min_clip_y;
			iy1 = y1;
		}else
		{
			iy1 = ceil(y1);
			xs += (iy1 - y1)*dxdyl;
			xe += (iy1 - y1)*dxdyr;
			zs += (iy1 - y1)*dzdyl;
			ze += (iy1 - y1)*dzdyr;
		}
		if (y3>=max_clip_y)
		{
			y3 = max_clip_y;
			iy3 = max_clip_y - 1;
		} 
		else
		{
			iy3 = ceil(y3) - 1;
		}
		lp_addr = lp_buffer + ((int)y1)*mempitch;
		lp_zaddr = lp_zbuffer + ((int)y1)*z_pitch;
		if(x1>=min_clip_x&&x1<=max_clip_x&&x2>=min_clip_x&&x2<=max_clip_x&&x3>=min_clip_x&&x3<=max_clip_x){
			for (loopy = iy1;loopy<=iy3;loopy++,lp_addr+=mempitch,lp_zaddr+=z_pitch)
			{
				dx = xe-xs+1;
				dzdx = (ze-zs)/dx;
				ixs = xs;
				for (loopx = 0;loopx<(int)dx;loopx++)
				{
					current_z = zs + loopx*dzdx;
					if (current_z < *(lp_zaddr+ixs+loopx))
					{
						Mem_Set_QUAD(lp_addr+ixs+loopx,poly->lit_color[0].argb,1);
						*(lp_zaddr+ixs+loopx) = current_z;
					}
				}
				xs += dxdyl;
				xe += dxdyr;
				zs += dzdyl;
				ze += dzdyr;
			}
		}else{
			float clip_xs,clip_xe,clip_zs,clip_ze;

			for (loopy = iy1;loopy<=iy3;loopy++,lp_addr+=mempitch,lp_zaddr+=z_pitch)
			{
				clip_xe = xe;
				clip_xs = xs;
				clip_ze = ze;
				clip_zs = zs;
				xs += dxdyl;
				xe += dxdyr;
				zs += dzdyl;
				ze += dzdyr;
				if (clip_xe<min_clip_x)
				{
					continue;
				}
				if (clip_xs>max_clip_x)
				{
					continue;
				}
				if (clip_xs<min_clip_x)
				{
					clip_xs = min_clip_x;
				}
				if (clip_xe>max_clip_x)
				{
					clip_xe = max_clip_x;
				}
				dx = clip_xe-clip_xs+1;
				dzdx = (ze-zs)/dx;
				zs += (clip_xe - xs)*dzdx;
				ixs = clip_xs;
				for (loopx = 0;loopx<(int)dx;loopx++)
				{
					current_z = zs + loopx*dzdx;
					if (current_z < *(lp_zaddr+ixs+loopx))
					{
						Mem_Set_QUAD(lp_addr+ixs+loopx,poly->lit_color[0].argb,1);
						*(lp_zaddr+ixs+loopx) = current_z;
					}
				}
			}
		}

		x1 = temp[3];y1 = temp[4];z1 = temp[5];
		x2 = temp[6];y2 = temp[7];z2 = temp[8];
		x3 = temp[9];y3 = temp[10];z3 = temp[11];
		if (x1>x2)
		{
			SWAP(x2,x1,temp_value);
			SWAP(y2,y1,temp_value);
			SWAP(z2,z1,temp_value);
		}
		
		dxdyl = (x3-x1)/(y3-y1);
		dxdyr = (x3-x2)/(y3-y2);
		dzdyl = (z3-z1)/(y3-y1);
		dzdyr = (z3-z2)/(y3-y2);
		xs = x1;
		xe = x2;
		zs = z1;
		ze = z2;

		if (y1<=min_clip_y)
		{
			xs += (min_clip_y - y1)*dxdyl;
			xe += (min_clip_y - y1)*dxdyr;
			zs += (min_clip_y - y1)*dzdyl;
			ze += (min_clip_y - y1)*dzdyr;
			y1 = min_clip_y;
			iy1 = y1;
		}else
		{
			iy1 = ceil(y1);
			xs += (iy1 - y1)*dxdyl;
			xe += (iy1 - y1)*dxdyr;
			zs += (iy1 - y1)*dzdyl;
			ze += (iy1 - y1)*dzdyr;
		}
		if (y3>=max_clip_y)
		{
			y3 = max_clip_y;
			iy3 = max_clip_y - 1;
		} 
		else
		{
			iy3 = ceil(y3) - 1;
		}
		lp_addr = lp_buffer + ((int)y1)*mempitch;
		lp_zaddr = lp_zbuffer + ((int)y1)*z_pitch;
		if(x1>=min_clip_x&&x1<=max_clip_x&&x2>=min_clip_x&&x2<=max_clip_x&&x3>=min_clip_x&&x3<=max_clip_x){
			for (loopy = iy1;loopy<=iy3;loopy++,lp_addr+=mempitch,lp_zaddr+=z_pitch)
			{
				dx = xe-xs+1;
				dzdx = (ze-zs)/dx;
				ixs = xs;
				for (loopx = 0;loopx<(int)dx;loopx++)
				{
					current_z = zs + loopx*dzdx;
					if (current_z < *(lp_zaddr+ixs+loopx))
					{
						Mem_Set_QUAD(lp_addr+ixs+loopx,poly->lit_color[0].argb,1);
						*(lp_zaddr+ixs+loopx) = current_z;
					}
				}
				xs += dxdyl;
				xe += dxdyr;
				zs += dzdyl;
				ze += dzdyr;
			}
		}else{
			float clip_xs,clip_xe,clip_zs,clip_ze;

			for (loopy = iy1;loopy<=iy3;loopy++,lp_addr+=mempitch,lp_zaddr+=z_pitch)
			{
				clip_xe = xe;
				clip_xs = xs;
				clip_ze = ze;
				clip_zs = zs;
				xs += dxdyl;
				xe += dxdyr;
				zs += dzdyl;
				ze += dzdyr;
				if (clip_xe<min_clip_x)
				{
					continue;
				}
				if (clip_xs>max_clip_x)
				{
					continue;
				}
				if (clip_xs<min_clip_x)
				{
					clip_xs = min_clip_x;
				}
				if (clip_xe>max_clip_x)
				{
					clip_xe = max_clip_x;
				}
				dx = clip_xe-clip_xs+1;
				dzdx = (ze-zs)/dx;
				zs += (clip_xs - xs)*dzdx;
				ixs = clip_xs;
				for (loopx = 0;loopx<(int)dx;loopx++)
				{
					current_z = zs + loopx*dzdx;
					if (current_z < *(lp_zaddr+ixs+loopx))
					{
						Mem_Set_QUAD(lp_addr+ixs+loopx,poly->lit_color[0].argb,1);
						*(lp_zaddr+ixs+loopx) = current_z;
					}
				}
			}
		}
		break;
	}
};

void Draw_Gouraud_Triangle_zb(RenderPoly* poly,UCHAR *dest_buffer, int mempitch,float* z_buffer,int z_pitch){
	Vertex3d data1;
	Vertex3d data2;
	Vertex3d data3;
	Vertex3d *v1;
	Vertex3d *v2;
	Vertex3d *v3;
	float x1,y1,x2,y2,x3,y3,temp_value;
	float z1,z2,z3;
	Color t_color,color1,color2,color3;
	mempitch>>=2;
	UINT* lp_buffer = (UINT*)dest_buffer;
	UINT* lp_addr = NULL;
	float* lp_zbuffer = z_buffer;
	float* lp_zaddr = NULL;
	int tri_type = TRI_TYPE_GENERAL;
	data1.pos = poly->tvlist[0].pos;
	data2.pos = poly->tvlist[1].pos;
	data3.pos = poly->tvlist[2].pos;
	data1.texture_pos =  poly->tvlist[0].texture_pos;
	data2.texture_pos =  poly->tvlist[1].texture_pos;
	data3.texture_pos =  poly->tvlist[2].texture_pos;
	v1 = &data1;
	v2 = &data2;
	v3 = &data3;
	Vertex3d *tv;
	if(v2->y<v1->y){
		SWAP(v1,v2,tv); 
		SWAP(poly->lit_color[0],poly->lit_color[1],t_color);
	}
	if (v3->y<v2->y)
	{
		SWAP(v2,v3,tv);
		SWAP(poly->lit_color[1],poly->lit_color[2],t_color);
	}
	if(v2->y<v1->y){
		SWAP(v1,v2,tv);
		SWAP(poly->lit_color[0],poly->lit_color[1],t_color);
	}
	if (FCMP(v2->y,v3->y))
	{
		tri_type = TRI_TYPE_BOTTOM;
		if (v2->x > v3->x)
		{
			SWAP(v2,v3,tv);
			SWAP(poly->lit_color[1],poly->lit_color[2],t_color);
		}
	}
	if (FCMP(v1->y,v2->y))
	{
		tri_type = TRI_TYPE_TOP;
		if (v1->x > v2->x)
		{
			SWAP(v1,v2,tv);
			SWAP(poly->lit_color[0],poly->lit_color[1],t_color);
		}
	}
	x1 = v1->x;y1 = v1->y;z1 = v1->z;color1 = poly->lit_color[0];
	x2 = v2->x;y2 = v2->y;z2 = v2->z;color2 = poly->lit_color[1];
	x3 = v3->x;y3 = v3->y;z3 = v3->z;color3 = poly->lit_color[2];
	if (y3<min_clip_y||y1>max_clip_y||
		(x1<min_clip_x&&x2<min_clip_x&&x3<min_clip_x)||
		(x1>max_clip_x&&x2>max_clip_x&&x3>max_clip_x))
	{
		return;
	}
	float dr_left,dg_left,db_left,dr_right,dg_right,db_right;
	float rs,gs,bs,rd,gd,bd;
	int iy1,iy3,loopy,loopx;
	int ixs;
	float y31,y32,y21;
	float loopx_dx;
	UINT final_color;
	float dxdyl,dzdyl,dxdyr,dzdyr,dy,xs,xe,dzdx,zs,ze,dx,current_z;
	switch(tri_type){
	case TRI_TYPE_BOTTOM:
		y21 = 1/(y2-y1);
		y31 = 1/(y3-y1);
		dxdyl = (x2-x1)*y21;
		dxdyr = (x3-x1)*y31;
		dzdyl = (z2-z1)*y21;
		dzdyr = (z3-z1)*y31;
		dr_left = (color2.r - color1.r)*y21;
		dg_left = (color2.g - color1.g)*y21;
		db_left = (color2.b - color1.b)*y21;
		dr_right = (color3.r - color1.r)*y31;
		dg_right = (color3.g - color1.g)*y31;
		db_right = (color3.b - color1.b)*y31;
		rs = color1.r;gs = color1.g;bs = color1.b;
		rd = color1.r;gd = color1.g;bd = color1.b;
		xs = x1;
		xe = x1;
		zs = z1;
		ze = z1;
		if (y1<=min_clip_y)
		{
			xs += (min_clip_y - y1)*dxdyl;
			xe += (min_clip_y - y1)*dxdyr;
			zs += (min_clip_y - y1)*dzdyl;
			ze += (min_clip_y - y1)*dzdyr;

			rs += (min_clip_y - y1)*dr_left;
			gs += (min_clip_y - y1)*dg_left;
			bs += (min_clip_y - y1)*db_left;
			rd += (min_clip_y - y1)*dr_right;
			gd += (min_clip_y - y1)*dg_right;
			bd += (min_clip_y - y1)*db_right;
			y1 = min_clip_y;
			iy1 = y1;
		} 
		else
		{
			iy1 = ceil(y1);
			xs += (iy1 - y1)*dxdyl;
			xe += (iy1 - y1)*dxdyr;
			zs += (iy1 - y1)*dzdyl;
			ze += (iy1 - y1)*dzdyr;
		}
		if (y3>=max_clip_y)
		{
			y3 = max_clip_y;
			iy3 = max_clip_y - 1;
		} 
		else
		{
			iy3 = ceil(y3) - 1;
		}
		lp_addr = lp_buffer + ((int)y1)*mempitch;
		lp_zaddr = lp_zbuffer + ((int)y1)*z_pitch;
		if(x1>=min_clip_x&&x1<=max_clip_x&&x2>=min_clip_x&&x2<=max_clip_x&&x3>=min_clip_x&&x3<=max_clip_x){
			for (loopy = iy1;loopy<=iy3;loopy++,lp_addr+=mempitch,lp_zaddr+=z_pitch)
			{
				dx = xe - xs + 1;
				dzdx = (ze-zs)/dx;
				ixs = xs;
				for (loopx = 0;loopx<dx;loopx++)
				{
					current_z = zs + loopx*dzdx;
					if (current_z < *(lp_zaddr+ixs+loopx))
					{
						loopx_dx = (loopx/dx);
						final_color = _RGB32BIT(0xff,(int)(rs + (rd-rs)*loopx_dx),(int)(gs + (gd-gs)*loopx_dx),(int)(bs + (bd-bs)*loopx_dx));
						Mem_Set_QUAD(lp_addr+ixs+loopx,final_color,1);
						*(lp_zaddr+ixs+loopx) = current_z;
					}					
				}
				xs += dxdyl;
				xe += dxdyr;
				zs += dzdyl;
				ze += dzdyr;
				rs += dr_left;
				gs += dg_left;
				bs += db_left;
				rd += dr_right;
				gd += dg_right;
				bd += db_right;
			}
		}else{
			float clip_xs,clip_xe,clip_zs,clip_ze;

			for (loopy = iy1;loopy<=iy3;loopy++,lp_addr+=mempitch,lp_zaddr+=z_pitch)
			{
				clip_xe = xe;
				clip_xs = xs;
				clip_ze = ze;
				clip_zs = zs;
				xs += dxdyl;
				xe += dxdyr;
				zs += dzdyl;
				ze += dzdyr;
				rs += dr_left;
				gs += dg_left;
				bs += db_left;
				rd += dr_right;
				gd += dg_right;
				bd += db_right;
				if (clip_xe<min_clip_x)
				{
					continue;
				}
				if (clip_xs>max_clip_x)
				{
					continue;
				}
				if (clip_xs<min_clip_x)
				{
					clip_xs = min_clip_x;
					rs += (min_clip_x - clip_xs)*dr_left;
					gs += (min_clip_x - clip_xs)*dg_left;
					bs += (min_clip_x - clip_xs)*db_left;
				}
				if (clip_xe>max_clip_x)
				{
					clip_xe = max_clip_x;
					rd -= (clip_xe - max_clip_x)*dr_right;
					gd -= (clip_xe - max_clip_x)*dg_right;
					bd -= (clip_xe - max_clip_x)*db_right;
				}
				dx = clip_xe - clip_xs + 1;
				dzdx = (ze-zs)/dx;
				zs += (clip_xs - xs)*dzdx;
				ixs = clip_xs;
				for (loopx = 0;loopx<(int)dx;loopx++)
				{
					current_z = zs + loopx*dzdx;
					if (current_z < *(lp_zaddr+ixs+loopx))
					{
						loopx_dx = (loopx/dx);
						final_color = _RGB32BIT(0xff,(int)(rs + (rd-rs)*loopx_dx),(int)(gs + (gd-gs)*loopx_dx),(int)(bs + (bd-bs)*loopx_dx));
						Mem_Set_QUAD(lp_addr+ixs+loopx,final_color,1);
						*(lp_zaddr+ixs+loopx) = current_z;
					}					
				}
			}		
		}
		break;
	case TRI_TYPE_TOP:
		y31 = 1/(y3-y1);
		y32 = 1/(y3-y2);
		dxdyl = (x3-x1)*y31;
		dxdyr = (x3-x2)*y32;
		dzdyl = (z3-z1)*y31;
		dzdyr = (z3-z2)*y32;
		dr_left = (color3.r - color1.r)*y31;
		dg_left = (color3.g - color1.g)*y31;
		db_left = (color3.b - color1.b)*y31;
		dr_right = (color3.r - color2.r)*y32;
		dg_right = (color3.g - color2.g)*y32;
		db_right = (color3.b - color2.b)*y32;
		rs = color1.r;gs = color1.g;bs = color1.b;
		rd = color2.r;gd = color2.g;bd = color2.b;
		xs = x1;
		xe = x2;
		zs = z1;
		ze = z2;
		if (y1<=min_clip_y)
		{
			xs += (min_clip_y - y1)*dxdyl;
			xe += (min_clip_y - y1)*dxdyr;
			zs += (min_clip_y - y1)*dzdyl;
			ze += (min_clip_y - y1)*dzdyr;

			rs += (min_clip_y - y1)*dr_left;
			gs += (min_clip_y - y1)*dg_left;
			bs += (min_clip_y - y1)*db_left;
			rd += (min_clip_y - y1)*dr_right;
			gd += (min_clip_y - y1)*dg_right;
			bd += (min_clip_y - y1)*db_right;
			y1 = min_clip_y;
			iy1 = y1;
		} 
		else
		{
			iy1 = ceil(y1);
			xs += (iy1 - y1)*dxdyl;
			xe += (iy1 - y1)*dxdyr;
			zs += (iy1 - y1)*dzdyl;
			ze += (iy1 - y1)*dzdyr;
		}
		if (y3>=max_clip_y)
		{
			y3 = max_clip_y;
			iy3 = max_clip_y - 1;
		} 
		else
		{
			iy3 = ceil(y3) - 1;
		}
		lp_addr = lp_buffer + ((int)y1)*mempitch;
		lp_zaddr = lp_zbuffer + ((int)y1)*z_pitch;
		if(x1>=min_clip_x&&x1<=max_clip_x&&x2>=min_clip_x&&x2<=max_clip_x&&x3>=min_clip_x&&x3<=max_clip_x){
			for (loopy = iy1;loopy<=iy3;loopy++,lp_addr+=mempitch,lp_zaddr+=z_pitch)
			{
				dx = xe - xs + 1;
				dzdx = (ze-zs)/dx;
				ixs = xs;
				for (loopx = 0;loopx<dx;loopx++)
				{
					current_z = zs + loopx*dzdx;
					if (current_z < *(lp_zaddr+ixs+loopx))
					{
						loopx_dx = (loopx/dx);
						final_color = _RGB32BIT(0xff,(int)(rs + (rd-rs)*loopx_dx),(int)(gs + (gd-gs)*loopx_dx),(int)(bs + (bd-bs)*loopx_dx));
						Mem_Set_QUAD(lp_addr+ixs+loopx,final_color,1);
						*(lp_zaddr+ixs+loopx) = current_z;
					}					
				}
				xs += dxdyl;
				xe += dxdyr;
				zs += dzdyl;
				ze += dzdyr;
				rs += dr_left;
				gs += dg_left;
				bs += db_left;
				rd += dr_right;
				gd += dg_right;
				bd += db_right;
			}
		}else{
			float clip_xs,clip_xe,clip_zs,clip_ze;

			for (loopy = iy1;loopy<=iy3;loopy++,lp_addr+=mempitch,lp_zaddr+=z_pitch)
			{
				clip_xe = xe;
				clip_xs = xs;
				clip_ze = ze;
				clip_zs = zs;
				xs += dxdyl;
				xe += dxdyr;
				zs += dzdyl;
				ze += dzdyr;
				rs += dr_left;
				gs += dg_left;
				bs += db_left;
				rd += dr_right;
				gd += dg_right;
				bd += db_right;
				if (clip_xe<min_clip_x)
				{
					continue;
				}
				if (clip_xs>max_clip_x)
				{
					continue;
				}
				if (clip_xs<min_clip_x)
				{
					clip_xs = min_clip_x;
					rs += (min_clip_x - clip_xs)*dr_left;
					gs += (min_clip_x - clip_xs)*dg_left;
					bs += (min_clip_x - clip_xs)*db_left;
				}
				if (clip_xe>max_clip_x)
				{
					clip_xe = max_clip_x;
					rd -= (clip_xe - max_clip_x)*dr_right;
					gd -= (clip_xe - max_clip_x)*dg_right;
					bd -= (clip_xe - max_clip_x)*db_right;
				}
				dx = clip_xe - clip_xs + 1;
				dzdx = (ze-zs)/dx;
				zs += (clip_xs - xs)*dzdx;
				ixs = clip_xs;
				for (loopx = 0;loopx<(int)dx;loopx++)
				{
					current_z = zs + loopx*dzdx;
					if (current_z < *(lp_zaddr+ixs+loopx))
					{
						loopx_dx = (loopx/dx);
						final_color = _RGB32BIT(0xff,(int)(rs + (rd-rs)*loopx_dx),(int)(gs + (gd-gs)*loopx_dx),(int)(bs + (bd-bs)*loopx_dx));
						Mem_Set_QUAD(lp_addr+ixs+loopx,final_color,1);
						*(lp_zaddr+ixs+loopx) = current_z;
					}					
				}
			}		
		}
		break;
	case TRI_TYPE_GENERAL:
		
		float temp[12] = {x1,y1,z1,x2,y2,z2,(y2-y1)*(x3-x1)/(y3-y1)+x1,y2,(y2-y1)*(z3-z1)/(y3-y1)+z1,x3,y3,z3};
		float d = (y3-y2)/(y3-y1);
		Color t_color;
		UINT t_argb;
		t_color.r = color3.r + (color1.r - color3.r)*d;
		t_color.g = color3.g + (color1.g - color3.g)*d;
		t_color.b = color3.b + (color1.b - color3.b)*d;
		Color colors[4] = {color1,color2,t_color,color3};
		x1 = temp[0];y1 = temp[1];z1 = temp[2];
		x2 = temp[3];y2 = temp[4];z2 = temp[5];
		x3 = temp[6];y3 = temp[7];z3 = temp[8];
		color1.argb = colors[0].argb;color2.argb = colors[1].argb;color3.argb = colors[2].argb;
		if (x2>x3)
		{
			SWAP(x2,x3,temp_value);
			SWAP(y2,y3,temp_value);
			SWAP(z2,z3,temp_value);
			SWAP(color2.argb,color3.argb,t_argb);
		}
		y21 = 1/(y2-y1);
		y31 = 1/(y3-y1);
		dxdyl = (x2-x1)*y21;
		dxdyr = (x3-x1)*y31;
		dzdyl = (z2-z1)*y21;
		dzdyr = (z3-z1)*y31;
		dr_left = (color2.r - color1.r)*y21;
		dg_left = (color2.g - color1.g)*y21;
		db_left = (color2.b - color1.b)*y21;
		dr_right = (color3.r - color1.r)*y31;
		dg_right = (color3.g - color1.g)*y31;
		db_right = (color3.b - color1.b)*y31;
		rs = color1.r;gs = color1.g;bs = color1.b;
		rd = color1.r;gd = color1.g;bd = color1.b;
		xs = x1;
		xe = x1;
		zs = z1;
		ze = z1;
		if (y1<=min_clip_y)
		{
			xs += (min_clip_y - y1)*dxdyl;
			xe += (min_clip_y - y1)*dxdyr;
			zs += (min_clip_y - y1)*dzdyl;
			ze += (min_clip_y - y1)*dzdyr;

			rs += (min_clip_y - y1)*dr_left;
			gs += (min_clip_y - y1)*dg_left;
			bs += (min_clip_y - y1)*db_left;
			rd += (min_clip_y - y1)*dr_right;
			gd += (min_clip_y - y1)*dg_right;
			bd += (min_clip_y - y1)*db_right;
			y1 = min_clip_y;
			iy1 = y1;
		} 
		else
		{
			iy1 = ceil(y1);
			xs += (iy1 - y1)*dxdyl;
			xe += (iy1 - y1)*dxdyr;
			zs += (iy1 - y1)*dzdyl;
			ze += (iy1 - y1)*dzdyr;
		}
		if (y3>=max_clip_y)
		{
			y3 = max_clip_y;
			iy3 = max_clip_y - 1;
		} 
		else
		{
			iy3 = ceil(y3) - 1;
		}
		lp_addr = lp_buffer + ((int)y1)*mempitch;
		lp_zaddr = lp_zbuffer + ((int)y1)*z_pitch;
		if(x1>=min_clip_x&&x1<=max_clip_x&&x2>=min_clip_x&&x2<=max_clip_x&&x3>=min_clip_x&&x3<=max_clip_x){
			for (loopy = iy1;loopy<=iy3;loopy++,lp_addr+=mempitch,lp_zaddr+=z_pitch)
			{
				dx = xe - xs + 1;
				dzdx = (ze-zs)/dx;
				ixs = xs;
				for (loopx = 0;loopx<dx;loopx++)
				{
					current_z = zs + loopx*dzdx;
					if (current_z < *(lp_zaddr+ixs+loopx))
					{
						loopx_dx = (loopx/dx);
						final_color = _RGB32BIT(0xff,(int)(rs + (rd-rs)*loopx_dx),(int)(gs + (gd-gs)*loopx_dx),(int)(bs + (bd-bs)*loopx_dx));
						Mem_Set_QUAD(lp_addr+ixs+loopx,final_color,1);
						*(lp_zaddr+ixs+loopx) = current_z;
					}					
				}
				xs += dxdyl;
				xe += dxdyr;
				zs += dzdyl;
				ze += dzdyr;
				rs += dr_left;
				gs += dg_left;
				bs += db_left;
				rd += dr_right;
				gd += dg_right;
				bd += db_right;
			}
		}else{
			float clip_xs,clip_xe,clip_zs,clip_ze;

			for (loopy = iy1;loopy<=iy3;loopy++,lp_addr+=mempitch,lp_zaddr+=z_pitch)
			{
				clip_xe = xe;
				clip_xs = xs;
				clip_ze = ze;
				clip_zs = zs;
				xs += dxdyl;
				xe += dxdyr;
				zs += dzdyl;
				ze += dzdyr;
				rs += dr_left;
				gs += dg_left;
				bs += db_left;
				rd += dr_right;
				gd += dg_right;
				bd += db_right;
				if (clip_xe<min_clip_x)
				{
					continue;
				}
				if (clip_xs>max_clip_x)
				{
					continue;
				}
				if (clip_xs<min_clip_x)
				{
					clip_xs = min_clip_x;
					rs += (min_clip_x - clip_xs)*dr_left;
					gs += (min_clip_x - clip_xs)*dg_left;
					bs += (min_clip_x - clip_xs)*db_left;
				}
				if (clip_xe>max_clip_x)
				{
					clip_xe = max_clip_x;
					rd -= (clip_xe - max_clip_x)*dr_right;
					gd -= (clip_xe - max_clip_x)*dg_right;
					bd -= (clip_xe - max_clip_x)*db_right;
				}
				dx = clip_xe - clip_xs + 1;
				dzdx = (ze-zs)/dx;
				zs += (clip_xs - xs)*dzdx;
				ixs = clip_xs;
				for (loopx = 0;loopx<(int)dx;loopx++)
				{
					current_z = zs + loopx*dzdx;
					if (current_z < *(lp_zaddr+ixs+loopx))
					{
						loopx_dx = (loopx/dx);
						final_color = _RGB32BIT(0xff,(int)(rs + (rd-rs)*loopx_dx),(int)(gs + (gd-gs)*loopx_dx),(int)(bs + (bd-bs)*loopx_dx));
						Mem_Set_QUAD(lp_addr+ixs+loopx,final_color,1);
						*(lp_zaddr+ixs+loopx) = current_z;
					}					
				}
			}		
		}
		x1 = temp[3];y1 = temp[4];z1 = temp[5];
		x2 = temp[6];y2 = temp[7];z2 = temp[8];
		x3 = temp[9];y3 = temp[10];z3 = temp[11];
		color1.argb = colors[1].argb;color2.argb = colors[2].argb;color3.argb = colors[3].argb;
		if (x1>x2)
		{
			SWAP(x2,x1,temp_value);
			SWAP(y2,y1,temp_value);
			SWAP(z2,z1,temp_value);
			SWAP(color2.argb,color1.argb,t_argb);
		}
		y31 = 1/(y3-y1);
		y32 = 1/(y3-y2);
		dxdyl = (x3-x1)*y31;
		dxdyr = (x3-x2)*y32;
		dzdyl = (z3-z1)*y31;
		dzdyr = (z3-z2)*y32;
		dr_left = (color3.r - color1.r)*y31;
		dg_left = (color3.g - color1.g)*y31;
		db_left = (color3.b - color1.b)*y31;
		dr_right = (color3.r - color2.r)*y32;
		dg_right = (color3.g - color2.g)*y32;
		db_right = (color3.b - color2.b)*y32;
		rs = color1.r;gs = color1.g;bs = color1.b;
		rd = color2.r;gd = color2.g;bd = color2.b;
		xs = x1;
		xe = x2;
		zs = z1;
		ze = z2;
		if (y1<=min_clip_y)
		{
			xs += (min_clip_y - y1)*dxdyl;
			xe += (min_clip_y - y1)*dxdyr;
			zs += (min_clip_y - y1)*dzdyl;
			ze += (min_clip_y - y1)*dzdyr;

			rs += (min_clip_y - y1)*dr_left;
			gs += (min_clip_y - y1)*dg_left;
			bs += (min_clip_y - y1)*db_left;
			rd += (min_clip_y - y1)*dr_right;
			gd += (min_clip_y - y1)*dg_right;
			bd += (min_clip_y - y1)*db_right;
			y1 = min_clip_y;
			iy1 = y1;
		} 
		else
		{
			iy1 = ceil(y1);
			xs += (iy1 - y1)*dxdyl;
			xe += (iy1 - y1)*dxdyr;
			zs += (iy1 - y1)*dzdyl;
			ze += (iy1 - y1)*dzdyr;
		}
		if (y3>=max_clip_y)
		{
			y3 = max_clip_y;
			iy3 = max_clip_y - 1;
		} 
		else
		{
			iy3 = ceil(y3) - 1;
		}
		if(x1>=min_clip_x&&x1<=max_clip_x&&x2>=min_clip_x&&x2<=max_clip_x&&x3>=min_clip_x&&x3<=max_clip_x){
			for (loopy = iy1;loopy<=iy3;loopy++,lp_addr+=mempitch,lp_zaddr+=z_pitch)
			{
				dx = xe - xs + 1;
				dzdx = (ze-zs)/dx;
				ixs = xs;
				for (loopx = 0;loopx<dx;loopx++)
				{
					current_z = zs + loopx*dzdx;
					if (current_z < *(lp_zaddr+ixs+loopx))
					{
						loopx_dx = (loopx/dx);
						final_color = _RGB32BIT(0xff,(int)(rs + (rd-rs)*loopx_dx),(int)(gs + (gd-gs)*loopx_dx),(int)(bs + (bd-bs)*loopx_dx));
						Mem_Set_QUAD(lp_addr+ixs+loopx,final_color,1);
						*(lp_zaddr+ixs+loopx) = current_z;
					}					
				}
				xs += dxdyl;
				xe += dxdyr;
				zs += dzdyl;
				ze += dzdyr;
				rs += dr_left;
				gs += dg_left;
				bs += db_left;
				rd += dr_right;
				gd += dg_right;
				bd += db_right;
			}
		}else{
			float clip_xs,clip_xe,clip_zs,clip_ze;

			for (loopy = iy1;loopy<=iy3;loopy++,lp_addr+=mempitch,lp_zaddr+=z_pitch)
			{
				clip_xe = xe;
				clip_xs = xs;
				clip_ze = ze;
				clip_zs = zs;
				xs += dxdyl;
				xe += dxdyr;
				zs += dzdyl;
				ze += dzdyr;
				rs += dr_left;
				gs += dg_left;
				bs += db_left;
				rd += dr_right;
				gd += dg_right;
				bd += db_right;
				if (clip_xe<min_clip_x)
				{
					continue;
				}
				if (clip_xs>max_clip_x)
				{
					continue;
				}
				if (clip_xs<min_clip_x)
				{
					clip_xs = min_clip_x;
					rs += (min_clip_x - clip_xs)*dr_left;
					gs += (min_clip_x - clip_xs)*dg_left;
					bs += (min_clip_x - clip_xs)*db_left;
				}
				if (clip_xe>max_clip_x)
				{
					clip_xe = max_clip_x;
					rd -= (clip_xe - max_clip_x)*dr_right;
					gd -= (clip_xe - max_clip_x)*dg_right;
					bd -= (clip_xe - max_clip_x)*db_right;
				}
				dx = clip_xe - clip_xs + 1;
				dzdx = (ze-zs)/dx;
				zs += (clip_xs - xs)*dzdx;
				ixs = clip_xs;
				for (loopx = 0;loopx<(int)dx;loopx++)
				{
					current_z = zs + loopx*dzdx;
					if (current_z < *(lp_zaddr+ixs+loopx))
					{
						loopx_dx = (loopx/dx);
						final_color = _RGB32BIT(0xff,(int)(rs + (rd-rs)*loopx_dx),(int)(gs + (gd-gs)*loopx_dx),(int)(bs + (bd-bs)*loopx_dx));
						Mem_Set_QUAD(lp_addr+ixs+loopx,final_color,1);
						*(lp_zaddr+ixs+loopx) = current_z;
					}					
				}
			}		
		}
		break;
	}
};

void drawTextureTriangle_zb(RenderPoly *poly,BitmapData *texture,UCHAR *dest_buffer, int mempitch,float* z_buffer,int z_pitch){
	Vertex3d data1;
	Vertex3d data2;
	Vertex3d data3;
	Vertex3d *v1;
	Vertex3d *v2;
	Vertex3d *v3;
	float x1,y1,x2,y2,x3,y3,tx1,tx2,tx3,ty1,ty2,ty3,temp_value;
	float z1,z2,z3;
	Color t_color,color1,color2,color3;
	mempitch>>=2;
	UINT* lp_buffer = (UINT*)dest_buffer;
	UINT* lp_addr = NULL;
	float* lp_zbuffer = z_buffer;
	float* lp_zaddr = NULL;
	int tri_type = TRI_TYPE_GENERAL;
	data1.pos = poly->tvlist[0].pos;
	data2.pos = poly->tvlist[1].pos;
	data3.pos = poly->tvlist[2].pos;
	data1.texture_pos =  poly->tvlist[0].texture_pos;
	data2.texture_pos =  poly->tvlist[1].texture_pos;
	data3.texture_pos =  poly->tvlist[2].texture_pos;
	v1 = &data1;
	v2 = &data2;
	v3 = &data3;
	Vertex3d *tv;
	if(v2->y<v1->y){
		SWAP(v1,v2,tv); 
		SWAP(poly->lit_color[0],poly->lit_color[1],t_color);
	}
	if (v3->y<v2->y)
	{
		SWAP(v2,v3,tv);
		SWAP(poly->lit_color[1],poly->lit_color[2],t_color);
	}
	if(v2->y<v1->y){
		SWAP(v1,v2,tv);
		SWAP(poly->lit_color[0],poly->lit_color[1],t_color);
	}
	if (FCMP(v2->y,v3->y))
	{
		tri_type = TRI_TYPE_BOTTOM;
		if (v2->x > v3->x)
		{
			SWAP(v2,v3,tv);
			SWAP(poly->lit_color[1],poly->lit_color[2],t_color);
		}
	}
	if (FCMP(v1->y,v2->y))
	{
		tri_type = TRI_TYPE_TOP;
		if (v1->x > v2->x)
		{
			SWAP(v1,v2,tv);
			SWAP(poly->lit_color[0],poly->lit_color[1],t_color);
		}
	}
	x1 = v1->x;y1 = v1->y;z1 = v1->z;
	x2 = v2->x;y2 = v2->y;z2 = v2->z;
	x3 = v3->x;y3 = v3->y;z3 = v3->z;
	tx1 = v1->tx;ty1 = v1->ty;
	tx2 = v2->tx;ty2 = v2->ty;
	tx3 = v3->tx;ty3 = v3->ty;
	if (y3<min_clip_y||y1>max_clip_y||
		(x1<min_clip_x&&x2<min_clip_x&&x3<min_clip_x)||
		(x1>max_clip_x&&x2>max_clip_x&&x3>max_clip_x))
	{
		return;
	}
	float dtx_left,dty_left,dtx_right,dty_right;
	int iy1,iy3,loopy,loopx;
	int ixs;
	float y31,y32,y21;
	float loopx_dx;
	UINT final_color;
	float txs,txe,tys,tye;
	float dxdyl,dzdyl,dxdyr,dzdyr,dy,xs,xe,dzdx,zs,ze,dx,current_z;
	int texture_x;
	int texture_y;
	float tx_step;
	float ty_step;
	float light_r = (float)poly->lit_color[0].r/256;
	float light_g = (float)poly->lit_color[0].g/256;
	float light_b = (float)poly->lit_color[0].b/256;
	Color temp_color;
	switch(tri_type){
	case TRI_TYPE_BOTTOM:
		y21 = 1/(y2-y1);
		y31 = 1/(y3-y1);
		dxdyl = (x2-x1)*y21;
		dxdyr = (x3-x1)*y31;
		dzdyl = (z2-z1)*y21;
		dzdyr = (z3-z1)*y31;
		dtx_left = (tx2 - tx1)*y21;
		dty_left = (ty2 - ty1)*y21;
		dtx_right = (tx3 - tx1)*y31;
		dty_right = (ty3 - ty1)*y31;
		xs = x1;
		xe = x1;
		zs = z1;
		ze = z1;
		txs = tx1;
		txe = tx1;
		tys = ty1;
		tye = ty1;
		if (y1<=min_clip_y)
		{
			xs += (min_clip_y - y1)*dxdyl;
			xe += (min_clip_y - y1)*dxdyr;
			zs += (min_clip_y - y1)*dzdyl;
			ze += (min_clip_y - y1)*dzdyr;
			txs += (min_clip_y - y1)*dtx_left;
			txe += (min_clip_y - y1)*dtx_right;
			tys += (min_clip_y - y1)*dty_left;
			tye += (min_clip_y - y1)*dty_right;

			y1 = min_clip_y;
			iy1 = y1;
		} 
		else
		{
			iy1 = ceil(y1);
			xs += (iy1 - y1)*dxdyl;
			xe += (iy1 - y1)*dxdyr;
			zs += (iy1 - y1)*dzdyl;
			ze += (iy1 - y1)*dzdyr;
			txs += (iy1 - y1)*dtx_left;
			txe += (iy1 - y1)*dtx_right;
			tys += (iy1 - y1)*dty_left;
			tye += (iy1 - y1)*dty_right;
		}
		if (y3>=max_clip_y)
		{
			y3 = max_clip_y;
			iy3 = max_clip_y - 1;
		} 
		else
		{
			iy3 = ceil(y3) - 1;
		}
		lp_addr = lp_buffer + ((int)y1)*mempitch;
		lp_zaddr = lp_zbuffer + ((int)y1)*z_pitch;
		if(x1>=min_clip_x&&x1<=max_clip_x&&x2>=min_clip_x&&x2<=max_clip_x&&x3>=min_clip_x&&x3<=max_clip_x){
			for (loopy = iy1;loopy<=iy3;loopy++,lp_addr+=mempitch,lp_zaddr+=z_pitch)
			{
				dx = xe - xs + 1;
				dzdx = (ze-zs)/dx;
				ixs = xs;
				tx_step = (txe - txs)/dx;
				ty_step = (tye - tys)/dx;
				for (loopx = 0;loopx<dx;loopx++)
				{
					current_z = zs + loopx*dzdx;
					if (current_z < *(lp_zaddr+ixs+loopx))
					{
						texture_x = (txs + loopx*tx_step)*(texture->width - 1);
						texture_y = (tys + loopx*ty_step)*(texture->height - 1);
						temp_color = texture->data[(texture_y<<texture->power_of_two)+texture_x];
						temp_color.r *= light_r;
						temp_color.g *= light_g;
						temp_color.b *= light_b;
						final_color = temp_color.argb;
						Mem_Set_QUAD(lp_addr+ixs+loopx,final_color,1);
						*(lp_zaddr+ixs+loopx) = current_z;
					}					
				}
				xs += dxdyl;
				xe += dxdyr;
				zs += dzdyl;
				ze += dzdyr;
				txs += dtx_left;
				txe += dtx_right;
				tys += dty_left;
				tye += dty_right;
			}
		}else{
			float clip_xs,clip_xe,clip_zs,clip_ze,clip_txs,clip_tys;

			for (loopy = iy1;loopy<=iy3;loopy++,lp_addr+=mempitch,lp_zaddr+=z_pitch)
			{
				clip_xe = xe;
				clip_xs = xs;
				clip_ze = ze;
				clip_zs = zs;
				clip_txs = txs;
				clip_tys = tys;
				tx_step = (txe - txs)/(clip_xe - clip_xs + 1);
				ty_step = (tye - tys)/(clip_xe - clip_xs + 1);
				xs += dxdyl;
				xe += dxdyr;
				zs += dzdyl;
				ze += dzdyr;
				txs += dtx_left;
				txe += dtx_right;
				tys += dty_left;
				tye += dty_right;
				if (clip_xe<min_clip_x)
				{
					continue;
				}
				if (clip_xs>max_clip_x)
				{
					continue;
				}
				if (clip_xs<min_clip_x)
				{
					clip_xs = min_clip_x;
					clip_txs += (min_clip_x - clip_xs)*tx_step;
					clip_tys += (min_clip_x - clip_xs)*ty_step;
				}
				if (clip_xe>max_clip_x)
				{
					clip_xe = max_clip_x;
				}
				dx = clip_xe - clip_xs + 1;
				dzdx = (ze-zs)/dx;
				zs += (clip_xs - xs)*dzdx;
				ixs = clip_xs;
				for (loopx = 0;loopx<(int)dx;loopx++)
				{
					current_z = zs + loopx*dzdx;
					if (current_z < *(lp_zaddr+ixs+loopx))
					{
						texture_x = (clip_txs + loopx*tx_step)*(texture->width - 1);
						texture_y = (clip_tys + loopx*ty_step)*(texture->height - 1);
						temp_color = texture->data[(texture_y<<texture->power_of_two)+texture_x];
						temp_color.r *= light_r;
						temp_color.g *= light_g;
						temp_color.b *= light_b;
						final_color = temp_color.argb;
						Mem_Set_QUAD(lp_addr+ixs+loopx,final_color,1);
						*(lp_zaddr+ixs+loopx) = current_z;
					}					
				}
			}		
		}
		break;
	case TRI_TYPE_TOP:
		y31 = 1/(y3-y1);
		y32 = 1/(y3-y2);
		dxdyl = (x3-x1)*y31;
		dxdyr = (x3-x2)*y32;
		dzdyl = (z3-z1)*y31;
		dzdyr = (z3-z2)*y32;
		dtx_left = (tx3 - tx1)*y31;
		dty_left = (ty3 - ty1)*y31;
		dtx_right = (tx3 - tx2)*y32;
		dty_right = (ty3 - ty2)*y32;
		xs = x1;
		xe = x2;
		zs = z1;
		ze = z2;
		txs = tx1;
		txe = tx2;
		tys = ty1;
		tye = ty2;
		if (y1<=min_clip_y)
		{
			xs += (min_clip_y - y1)*dxdyl;
			xe += (min_clip_y - y1)*dxdyr;
			zs += (min_clip_y - y1)*dzdyl;
			ze += (min_clip_y - y1)*dzdyr;

			txs += (min_clip_y - y1)*dtx_left;
			txe += (min_clip_y - y1)*dtx_right;
			tys += (min_clip_y - y1)*dty_left;
			tye += (min_clip_y - y1)*dty_right;
			y1 = min_clip_y;
			iy1 = y1;
		} 
		else
		{
			iy1 = ceil(y1);
			xs += (iy1 - y1)*dxdyl;
			xe += (iy1 - y1)*dxdyr;
			zs += (iy1 - y1)*dzdyl;
			ze += (iy1 - y1)*dzdyr;
			txs += (iy1 - y1)*dtx_left;
			txe += (iy1 - y1)*dtx_right;
			tys += (iy1 - y1)*dty_left;
			tye += (iy1 - y1)*dty_right;
		}
		if (y3>=max_clip_y)
		{
			y3 = max_clip_y;
			iy3 = max_clip_y - 1;
		} 
		else
		{
			iy3 = ceil(y3) - 1;
		}
		lp_addr = lp_buffer + ((int)y1)*mempitch;
		lp_zaddr = lp_zbuffer + ((int)y1)*z_pitch;
		if(x1>=min_clip_x&&x1<=max_clip_x&&x2>=min_clip_x&&x2<=max_clip_x&&x3>=min_clip_x&&x3<=max_clip_x){
			for (loopy = iy1;loopy<=iy3;loopy++,lp_addr+=mempitch,lp_zaddr+=z_pitch)
			{
				dx = xe - xs + 1;
				dzdx = (ze-zs)/dx;
				ixs = xs;
				tx_step = (txe - txs)/dx;
				ty_step = (tye - tys)/dx;
				for (loopx = 0;loopx<dx;loopx++)
				{
					current_z = zs + loopx*dzdx;
					if (current_z < *(lp_zaddr+ixs+loopx))
					{
						texture_x = (txs + loopx*tx_step)*(texture->width - 1);
						texture_y = (tys + loopx*ty_step)*(texture->height - 1);
						temp_color = texture->data[(texture_y<<texture->power_of_two)+texture_x];
						temp_color.r *= light_r;
						temp_color.g *= light_g;
						temp_color.b *= light_b;
						final_color = temp_color.argb;
						Mem_Set_QUAD(lp_addr+ixs+loopx,final_color,1);
						*(lp_zaddr+ixs+loopx) = current_z;
					}					
				}
				xs += dxdyl;
				xe += dxdyr;
				zs += dzdyl;
				ze += dzdyr;
				txs += dtx_left;
				txe += dtx_right;
				tys += dty_left;
				tye += dty_right;
			}
		}else{
			float clip_xs,clip_xe,clip_zs,clip_ze,clip_txs,clip_tys;

			for (loopy = iy1;loopy<=iy3;loopy++,lp_addr+=mempitch,lp_zaddr+=z_pitch)
			{
				clip_xe = xe;
				clip_xs = xs;
				clip_ze = ze;
				clip_zs = zs;
				clip_txs = txs;
				clip_tys = tys;
				tx_step = (txe - txs)/(clip_xe - clip_xs + 1);
				ty_step = (tye - tys)/(clip_xe - clip_xs + 1);
				xs += dxdyl;
				xe += dxdyr;
				zs += dzdyl;
				ze += dzdyr;
				txs += dtx_left;
				txe += dtx_right;
				tys += dty_left;
				tye += dty_right;
				if (clip_xe<min_clip_x)
				{
					continue;
				}
				if (clip_xs>max_clip_x)
				{
					continue;
				}
				if (clip_xs<min_clip_x)
				{
					clip_xs = min_clip_x;
					clip_txs += (min_clip_x - clip_xs)*tx_step;
					clip_tys += (min_clip_x - clip_xs)*ty_step;
				}
				if (clip_xe>max_clip_x)
				{
					clip_xe = max_clip_x;
				}
				dx = clip_xe - clip_xs + 1;
				dzdx = (ze-zs)/dx;
				zs += (clip_xs - xs)*dzdx;
				ixs = clip_xs;
				for (loopx = 0;loopx<(int)dx;loopx++)
				{
					current_z = zs + loopx*dzdx;
					if (current_z < *(lp_zaddr+ixs+loopx))
					{
						texture_x = (clip_txs + loopx*tx_step)*(texture->width - 1);
						texture_y = (clip_tys + loopx*ty_step)*(texture->height - 1);
						temp_color = texture->data[(texture_y<<texture->power_of_two)+texture_x];
						temp_color.r *= light_r;
						temp_color.g *= light_g;
						temp_color.b *= light_b;
						final_color = temp_color.argb;
						Mem_Set_QUAD(lp_addr+ixs+loopx,final_color,1);
						*(lp_zaddr+ixs+loopx) = current_z;
					}					
				}
			}		
		}
		break;
	case TRI_TYPE_GENERAL:

		float temp[12] = {x1,y1,z1,x2,y2,z2,(y2-y1)*(x3-x1)/(y3-y1)+x1,y2,(y2-y1)*(z3-z1)/(y3-y1)+z1,x3,y3,z3};
		float temp_txy[8] = {tx1,ty1,tx2,ty2,(y2-y1)*(tx3-tx1)/(y3-y1)+tx1,(y2-y1)*(ty3-ty1)/(y3-y1)+ty1,tx3,ty3};
		UINT t_argb;
		x1 = temp[0];y1 = temp[1];z1 = temp[2];
		x2 = temp[3];y2 = temp[4];z2 = temp[5];
		x3 = temp[6];y3 = temp[7];z3 = temp[8];
		tx1 = temp_txy[0];ty1 = temp_txy[1];
		tx2 = temp_txy[2];ty2 = temp_txy[3];
		tx3 = temp_txy[4];ty3 = temp_txy[5];
		if (x2>x3)
		{
			SWAP(x2,x3,temp_value);
			SWAP(y2,y3,temp_value);
			SWAP(z2,z3,temp_value);
			SWAP(tx2,tx3,temp_value);
			SWAP(ty2,ty3,temp_value);
		}
		y21 = 1/(y2-y1);
		y31 = 1/(y3-y1);
		dxdyl = (x2-x1)*y21;
		dxdyr = (x3-x1)*y31;
		dzdyl = (z2-z1)*y21;
		dzdyr = (z3-z1)*y31;
		dtx_left = (tx2 - tx1)*y21;
		dty_left = (ty2 - ty1)*y21;
		dtx_right = (tx3 - tx1)*y31;
		dty_right = (ty3 - ty1)*y31;
		xs = x1;
		xe = x1;
		zs = z1;
		ze = z1;
		txs = tx1;
		txe = tx1;
		tys = ty1;
		tye = ty1;
		if (y1<=min_clip_y)
		{
			xs += (min_clip_y - y1)*dxdyl;
			xe += (min_clip_y - y1)*dxdyr;
			zs += (min_clip_y - y1)*dzdyl;
			ze += (min_clip_y - y1)*dzdyr;
			txs += (min_clip_y - y1)*dtx_left;
			txe += (min_clip_y - y1)*dtx_right;
			tys += (min_clip_y - y1)*dty_left;
			tye += (min_clip_y - y1)*dty_right;

			y1 = min_clip_y;
			iy1 = y1;
		} 
		else
		{
			iy1 = ceil(y1);
			xs += (iy1 - y1)*dxdyl;
			xe += (iy1 - y1)*dxdyr;
			zs += (iy1 - y1)*dzdyl;
			ze += (iy1 - y1)*dzdyr;
			txs += (iy1 - y1)*dtx_left;
			txe += (iy1 - y1)*dtx_right;
			tys += (iy1 - y1)*dty_left;
			tye += (iy1 - y1)*dty_right;
		}
		if (y3>=max_clip_y)
		{
			y3 = max_clip_y;
			iy3 = max_clip_y - 1;
		} 
		else
		{
			iy3 = ceil(y3) - 1;
		}
		lp_addr = lp_buffer + ((int)y1)*mempitch;
		lp_zaddr = lp_zbuffer + ((int)y1)*z_pitch;
		if(x1>=min_clip_x&&x1<=max_clip_x&&x2>=min_clip_x&&x2<=max_clip_x&&x3>=min_clip_x&&x3<=max_clip_x){
			for (loopy = iy1;loopy<=iy3;loopy++,lp_addr+=mempitch,lp_zaddr+=z_pitch)
			{
				dx = xe - xs + 1;
				dzdx = (ze-zs)/dx;
				ixs = xs;
				tx_step = (txe - txs)/dx;
				ty_step = (tye - tys)/dx;
				for (loopx = 0;loopx<dx;loopx++)
				{
					current_z = zs + loopx*dzdx;
					if (current_z < *(lp_zaddr+ixs+loopx))
					{
						texture_x = (txs + loopx*tx_step)*(texture->width - 1);
						texture_y = (tys + loopx*ty_step)*(texture->height - 1);
						temp_color = texture->data[(texture_y<<texture->power_of_two)+texture_x];
						temp_color.r *= light_r;
						temp_color.g *= light_g;
						temp_color.b *= light_b;
						final_color = temp_color.argb;
						Mem_Set_QUAD(lp_addr+ixs+loopx,final_color,1);
						*(lp_zaddr+ixs+loopx) = current_z;
					}					
				}
				xs += dxdyl;
				xe += dxdyr;
				zs += dzdyl;
				ze += dzdyr;
				txs += dtx_left;
				txe += dtx_right;
				tys += dty_left;
				tye += dty_right;
			}
		}else{
			float clip_xs,clip_xe,clip_zs,clip_ze,clip_txs,clip_tys;

			for (loopy = iy1;loopy<=iy3;loopy++,lp_addr+=mempitch,lp_zaddr+=z_pitch)
			{
				clip_xe = xe;
				clip_xs = xs;
				clip_ze = ze;
				clip_zs = zs;
				clip_txs = txs;
				clip_tys = tys;
				tx_step = (txe - txs)/(clip_xe - clip_xs + 1);
				ty_step = (tye - tys)/(clip_xe - clip_xs + 1);
				xs += dxdyl;
				xe += dxdyr;
				zs += dzdyl;
				ze += dzdyr;
				txs += dtx_left;
				txe += dtx_right;
				tys += dty_left;
				tye += dty_right;
				if (clip_xe<min_clip_x)
				{
					continue;
				}
				if (clip_xs>max_clip_x)
				{
					continue;
				}
				if (clip_xs<min_clip_x)
				{
					clip_xs = min_clip_x;
					clip_txs += (min_clip_x - clip_xs)*tx_step;
					clip_tys += (min_clip_x - clip_xs)*ty_step;
				}
				if (clip_xe>max_clip_x)
				{
					clip_xe = max_clip_x;
				}
				dx = clip_xe - clip_xs + 1;
				dzdx = (ze-zs)/dx;
				zs += (clip_xs - xs)*dzdx;
				ixs = clip_xs;
				for (loopx = 0;loopx<(int)dx;loopx++)
				{
					current_z = zs + loopx*dzdx;
					if (current_z < *(lp_zaddr+ixs+loopx))
					{
						texture_x = (clip_txs + loopx*tx_step)*(texture->width - 1);
						texture_y = (clip_tys + loopx*ty_step)*(texture->height - 1);
						temp_color = texture->data[(texture_y<<texture->power_of_two)+texture_x];
						temp_color.r *= light_r;
						temp_color.g *= light_g;
						temp_color.b *= light_b;
						final_color = temp_color.argb;
						Mem_Set_QUAD(lp_addr+ixs+loopx,final_color,1);
						*(lp_zaddr+ixs+loopx) = current_z;
					}					
				}
			}		
		}
		x1 = temp[3];y1 = temp[4];z1 = temp[5];
		x2 = temp[6];y2 = temp[7];z2 = temp[8];
		x3 = temp[9];y3 = temp[10];z3 = temp[11];
		tx1 = temp_txy[2];ty1 = temp_txy[3];
		tx2 = temp_txy[4];ty2 = temp_txy[5];
		tx3 = temp_txy[6];ty3 = temp_txy[7];
		if (x1>x2)
		{
			SWAP(x2,x1,temp_value);
			SWAP(y2,y1,temp_value);
			SWAP(z2,z1,temp_value);
			SWAP(tx2,tx1,temp_value);
			SWAP(ty2,ty1,temp_value);
		}
		y31 = 1/(y3-y1);
		y32 = 1/(y3-y2);
		dxdyl = (x3-x1)*y31;
		dxdyr = (x3-x2)*y32;
		dzdyl = (z3-z1)*y31;
		dzdyr = (z3-z2)*y32;
		dtx_left = (tx3 - tx1)*y31;
		dty_left = (ty3 - ty1)*y31;
		dtx_right = (tx3 - tx2)*y32;
		dty_right = (ty3 - ty2)*y32;
		xs = x1;
		xe = x2;
		zs = z1;
		ze = z2;
		txs = tx1;
		txe = tx2;
		tys = ty1;
		tye = ty2;
		if (y1<=min_clip_y)
		{
			xs += (min_clip_y - y1)*dxdyl;
			xe += (min_clip_y - y1)*dxdyr;
			zs += (min_clip_y - y1)*dzdyl;
			ze += (min_clip_y - y1)*dzdyr;

			txs += (min_clip_y - y1)*dtx_left;
			txe += (min_clip_y - y1)*dtx_right;
			tys += (min_clip_y - y1)*dty_left;
			tye += (min_clip_y - y1)*dty_right;
			y1 = min_clip_y;
			iy1 = y1;
		} 
		else
		{
			iy1 = ceil(y1);
			xs += (iy1 - y1)*dxdyl;
			xe += (iy1 - y1)*dxdyr;
			zs += (iy1 - y1)*dzdyl;
			ze += (iy1 - y1)*dzdyr;
			txs += (iy1 - y1)*dtx_left;
			txe += (iy1 - y1)*dtx_right;
			tys += (iy1 - y1)*dty_left;
			tye += (iy1 - y1)*dty_right;
		}
		if (y3>=max_clip_y)
		{
			y3 = max_clip_y;
			iy3 = max_clip_y - 1;
		} 
		else
		{
			iy3 = ceil(y3) - 1;
		}
		lp_addr = lp_buffer + ((int)y1)*mempitch;
		lp_zaddr = lp_zbuffer + ((int)y1)*z_pitch;
		if(x1>=min_clip_x&&x1<=max_clip_x&&x2>=min_clip_x&&x2<=max_clip_x&&x3>=min_clip_x&&x3<=max_clip_x){
			for (loopy = iy1;loopy<=iy3;loopy++,lp_addr+=mempitch,lp_zaddr+=z_pitch)
			{
				dx = xe - xs + 1;
				dzdx = (ze-zs)/dx;
				ixs = xs;
				tx_step = (txe - txs)/dx;
				ty_step = (tye - tys)/dx;
				for (loopx = 0;loopx<dx;loopx++)
				{
					current_z = zs + loopx*dzdx;
					if (current_z < *(lp_zaddr+ixs+loopx))
					{
						texture_x = (txs + loopx*tx_step)*(texture->width - 1);
						texture_y = (tys + loopx*ty_step)*(texture->height - 1);
						temp_color = texture->data[(texture_y<<texture->power_of_two)+texture_x];
						temp_color.r *= light_r;
						temp_color.g *= light_g;
						temp_color.b *= light_b;
						final_color = temp_color.argb;
						Mem_Set_QUAD(lp_addr+ixs+loopx,final_color,1);
						*(lp_zaddr+ixs+loopx) = current_z;
					}					
				}
				xs += dxdyl;
				xe += dxdyr;
				zs += dzdyl;
				ze += dzdyr;
				txs += dtx_left;
				txe += dtx_right;
				tys += dty_left;
				tye += dty_right;
			}
		}else{
			float clip_xs,clip_xe,clip_zs,clip_ze,clip_txs,clip_tys;

			for (loopy = iy1;loopy<=iy3;loopy++,lp_addr+=mempitch,lp_zaddr+=z_pitch)
			{
				clip_xe = xe;
				clip_xs = xs;
				clip_ze = ze;
				clip_zs = zs;
				clip_txs = txs;
				clip_tys = tys;
				tx_step = (txe - txs)/(clip_xe - clip_xs + 1);
				ty_step = (tye - tys)/(clip_xe - clip_xs + 1);
				xs += dxdyl;
				xe += dxdyr;
				zs += dzdyl;
				ze += dzdyr;
				txs += dtx_left;
				txe += dtx_right;
				tys += dty_left;
				tye += dty_right;
				if (clip_xe<min_clip_x)
				{
					continue;
				}
				if (clip_xs>max_clip_x)
				{
					continue;
				}
				if (clip_xs<min_clip_x)
				{
					clip_xs = min_clip_x;
					clip_txs += (min_clip_x - clip_xs)*tx_step;
					clip_tys += (min_clip_x - clip_xs)*ty_step;
				}
				if (clip_xe>max_clip_x)
				{
					clip_xe = max_clip_x;
				}
				dx = clip_xe - clip_xs + 1;
				dzdx = (ze-zs)/dx;
				zs += (clip_xs - xs)*dzdx;
				ixs = clip_xs;
				for (loopx = 0;loopx<(int)dx;loopx++)
				{
					current_z = zs + loopx*dzdx;
					if (current_z < *(lp_zaddr+ixs+loopx))
					{
						texture_x = (clip_txs + loopx*tx_step)*(texture->width - 1);
						texture_y = (clip_tys + loopx*ty_step)*(texture->height - 1);
						temp_color = texture->data[(texture_y<<texture->power_of_two)+texture_x];
						temp_color.r *= light_r;
						temp_color.g *= light_g;
						temp_color.b *= light_b;
						final_color = temp_color.argb;
						Mem_Set_QUAD(lp_addr+ixs+loopx,final_color,1);
						*(lp_zaddr+ixs+loopx) = current_z;
					}					
				}
			}		
		}
		break;
	}
};