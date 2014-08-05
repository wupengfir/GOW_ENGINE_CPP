#include "GraphicFunctions.h"
int drawLine(int x0, int y0, int x1, int y1, UINT color)
{
	Clip_Line(x0,y0,x1,y1);
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