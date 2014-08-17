#include "GraphicFunctions.h"



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
	left = (x2-x1)/(y2-y1);
	right = (x3-x1)/(y3-y1);
	dr_left = (color1.r - color0.r)/(y2-y1);
	dg_left = (color1.g - color0.g)/(y2-y1);
	db_left = (color1.b - color0.b)/(y2-y1);
	dr_right = (color2.r - color0.r)/(y2-y1);
	dg_right = (color2.g - color0.g)/(y2-y1);
	db_right = (color2.b - color0.b)/(y2-y1);
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

	left = (x2-x1)/(y2-y1);
	right = (x3-x1)/(y3-y1);
	xs = x1;
	xe = x1;
	dr_left = (color0.r - color1.r)/(y2-y1);
	dg_left = (color0.g - color1.g)/(y2-y1);
	db_left = (color0.b - color1.b)/(y2-y1);
	dr_right = (color0.r - color2.r)/(y2-y1);
	dg_right = (color0.g - color2.g)/(y2-y1);
	db_right = (color0.b - color2.b)/(y2-y1);
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

