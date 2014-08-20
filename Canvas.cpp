#include "Canvas.h"

Canvas::Canvas(void)
{
	lp_camera = NULL;
	renderlist_all = new RenderList();
}

void Canvas::reset(){
	lp_directdraw = NULL;
	lp_primary_surface = NULL;
	lp_back_surface = NULL;
	lp_backbuffer = NULL;
	main_handler = 0;
	main_instance = 0;
}

void Canvas::flip(){
	if (lp_backbuffer)
	{
		return;
	} 
	else
	{
		if (is_window)
		{
			RECT    dest_rect;    // used to compute destination rectangle

			// get the window's rectangle in screen coordinates
			GetWindowRect(main_handler, &dest_rect);   

			// compute the destination rectangle
			dest_rect.left   +=window_client_x0;
			dest_rect.top    +=window_client_y0;

			dest_rect.right  =dest_rect.left+width-1;
			dest_rect.bottom =dest_rect.top +height-1;
			lp_primary_surface->Blt(&dest_rect, lp_back_surface,NULL,DDBLT_WAIT,NULL);
		} 
		else
		{
			lp_primary_surface->Flip(NULL, DDFLIP_WAIT);
		}
	}
}

void Canvas::clear(){	
	if (!lp_backbuffer)//��ס��ʱ�������
	{
		return;
	}
	if (lpitch == width)
	{
		memset((void*)lp_backbuffer,0,width*height*4);
	} 
	else
	{
		for (int i = 0;i<height;i++)
		{
			memset((void*)(lp_backbuffer+i*lpitch),0,width<<2);
		}
	}
}

UCHAR* Canvas::lock(){
	if (lp_backbuffer)
	{
		return lp_backbuffer;
	}
	DD_INIT_STRUCT(ddsd);
	lp_back_surface->Lock(NULL,&ddsd,DDLOCK_WAIT | DDLOCK_SURFACEMEMORYPTR,NULL);
	lp_backbuffer = (UCHAR*)ddsd.lpSurface;
	lpitch = ddsd.lPitch;
	return lp_backbuffer;
}

void Canvas::unlock(){
	if (!lp_back_surface)
	{
		return;
	}
	lp_back_surface->Unlock(NULL);
	lp_backbuffer = NULL;
	lpitch = 0;
}

void Canvas::init(HINSTANCE hinstance,WNDPROC callback,int width,int height,int bpp,int windowed){
	reset();
	main_instance = hinstance;
	is_window = windowed;
	this->bpp = bpp;
	this->width = width;
	this->height = height;
	WNDCLASSEX winclass; // this will hold the class we create
	// first fill in the window class stucture
	winclass.cbSize         = sizeof(WNDCLASSEX);
	winclass.style			= CS_DBLCLKS | CS_OWNDC | 
		CS_HREDRAW | CS_VREDRAW;
	winclass.lpfnWndProc	= callback;
	winclass.cbClsExtra		= 0;
	winclass.cbWndExtra		= 0;
	winclass.hInstance		= hinstance;
	winclass.hIcon			= LoadIcon(NULL, IDI_APPLICATION);
	winclass.hCursor		= LoadCursor(NULL, IDC_ARROW); 
	winclass.hbrBackground	= (HBRUSH)GetStockObject(BLACK_BRUSH);
	winclass.lpszMenuName	= NULL;
	winclass.lpszClassName	= WINDOW_CLASS_NAME;
	winclass.hIconSm        = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&winclass))
		return;
	if (windowed)
	{
		main_handler =  CreateWindowEx(NULL,                  // extended style
			WINDOW_CLASS_NAME,     // class
			"Demo", // title
			WS_OVERLAPPED | WS_VISIBLE,
			0,0,	  // initial x,y
			width,height,  // initial width, height
			NULL,	  // handle to parent 
			NULL,	  // handle to menu
			hinstance,// instance of this application
			NULL);

		RECT window_rect = {0,0,width-1,height-1};

		// make the call to adjust window_rect
		AdjustWindowRectEx(&window_rect,
			GetWindowStyle(main_handler),
			GetMenu(main_handler) != NULL,  
			GetWindowExStyle(main_handler));

		// save the global client offsets, they are needed in DDraw_Flip()
		window_client_x0 = -window_rect.left;
		window_client_y0 = -window_rect.top;

		// now resize the window with a call to MoveWindow()
		MoveWindow(main_handler,
			0, // x position
			0, // y position
			window_rect.right - window_rect.left, // width
			window_rect.bottom - window_rect.top, // height
			FALSE);

		// show the window, so there's no garbage on first render
		ShowWindow(main_handler, SW_SHOW);
	}else{
		main_handler = CreateWindowEx(NULL,                  // extended style
			WINDOW_CLASS_NAME,     // class
			"DirectDraw Full-Screen Demo", // title
			WS_POPUP | WS_VISIBLE,
			0,0,	  // initial x,y
			width,height,  // initial width, height
			NULL,	  // handle to parent 
			NULL,	  // handle to menu
			hinstance,// instance of this application
			NULL);
	}
	getDevice();
}

void Canvas::getDevice(){
	DD_INIT_STRUCT(ddsd);
	DirectDrawCreateEx(NULL,(LPVOID*)&lp_directdraw,IID_IDirectDraw7,NULL);
	if (is_window)
	{
		lp_directdraw->SetCooperativeLevel(main_handler,DDSCL_NORMAL);
		ddsd.dwFlags = DDSD_CAPS;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
	} 
	else
	{
		lp_directdraw->SetCooperativeLevel(main_handler,DDSCL_FULLSCREEN | DDSCL_ALLOWMODEX | DDSCL_EXCLUSIVE | DDSCL_ALLOWREBOOT);
		lp_directdraw->SetDisplayMode(width,height,bpp,0,0);
		
		ddsd.dwFlags = DDSD_CAPS|DDSD_BACKBUFFERCOUNT;
		ddsd.dwBackBufferCount = 1;
		ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE|DDSCAPS_COMPLEX|DDSCAPS_FLIP;
	}
	lp_directdraw->CreateSurface(&ddsd,&lp_primary_surface,NULL);
	//������ֱ�Ӿ�ȡddsd��lpitch��Ӧ����lock���ddsdȡ��
	if(is_window){
		DD_INIT_STRUCT(ddsd);
		ddsd.dwFlags = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
		//������DDSCAPS_VIDEOMEMORY������˸����DDSCAPS_SYSTEMMEMORY�Ļ����ܻ���˸
		ddsd.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY;
		ddsd.dwWidth = width;
		ddsd.dwHeight = height;
		lp_directdraw->CreateSurface(&ddsd,&lp_back_surface,NULL);
	}else{
		ddsd.ddsCaps.dwCaps = DDSCAPS_BACKBUFFER;
		lp_primary_surface->GetAttachedSurface(&(ddsd.ddsCaps),&lp_back_surface);
	}
	max_clip_x = width -1;
	max_clip_y = height - 1;
}

void Canvas::setCamera(Camera* camera){
	this->lp_camera = camera;
};

void Canvas::addObject(Object3d* obj){
	this->obj_list.push_back(obj);
};

void Canvas::addRenderList(RenderList* list){
	this->renderlist_list.push_back(list);
};

void Canvas::render(bool backmove,bool cull){
	int i = 0;
	if (lp_camera == NULL)
	{
		return;
	}
	renderlist_all->reset();
	if(lp_camera->attr == Camera::CAMERA_TYPE_EULER){
		lp_camera->buildWorldToCameraMatrix_Euler();
	}else{
		lp_camera->buildWorldToCameraMatrix_unv(UNV_MODE_SIMPLE);
	}
	Object3d *obj;
	for (std::vector<Object3d*>::size_type s = 0;s!=obj_list.size();s++)
	{
		obj = obj_list[s];
		obj->toWorldPosition(TRANSFORM_LOCAL_TO_TRANS);
		if (backmove)
		{
			lp_camera->removeBackFaceOfObj(obj);
		}
		if (cull)
		{
			lp_camera->cullObject(obj);
		}
		if (obj->state&OBJECT_STATE_CLIPPED)
		{
			continue;
		}
		lightObject(obj);//���շ�����������ΪҪ������ռ䴦����գ�����ĺ��������һ��flag����Ҫ��Ҫƫת���㷨�ߣ�Ҳ���Ի�λ��(��������)
		transformObject(obj,&(lp_camera->mcam),TRANSFORM_TRANS_ONLY,false,false);
		lp_camera->cameraToPerspective_object(obj);
		lp_camera->perspectiveToScreen_object(obj);
		Poly* temp;
		Point3d* p1;Point3d* p2;Point3d* p3;
		for (i = 0;i<obj->num_polys;i++)
		{
			temp = obj->lp_polys + i;
			if(temp != NULL&&temp->avaliable()){
				temp->calculateAvgZ();
				renderlist_all->addPoly(temp);
			}
		}
		for (i = 0;i<renderlist_all->num_polys;i++)
		{
			temp = renderlist_all->polys[i];
			p1 = &((temp->lp_vertex_object[temp->v_index_list[0]]).pos);
			p2 = &((temp->lp_vertex_object[temp->v_index_list[1]]).pos);
			p3 = &((temp->lp_vertex_object[temp->v_index_list[2]]).pos);
			/*drawLine(p1->x,p1->y,p2->x,p2->y,0xff00ffff);
			drawLine(p2->x,p2->y,p3->x,p3->y,0xff00ffff);
			drawLine(p3->x,p3->y,p1->x,p1->y,0xff00ffff);*/
			if (temp->attr&POLY4D_ATTR_SHADE_MODE_FLAT)
			{
				Draw_Triangle_2D(p1->x,p1->y,p2->x,p2->y,p3->x,p3->y,temp->lit_color[0].argb,lp_canvas->lp_backbuffer,lp_canvas->lpitch);
			}
			else if (temp->attr&POLY4D_ATTR_SHADE_MODE_GOURAUD)
			{
				Draw_Gouraud_Triangle(p1->x,p1->y,p2->x,p2->y,p3->x,p3->y,temp->lit_color[0],temp->lit_color[1],temp->lit_color[2],lp_canvas->lp_backbuffer,lp_canvas->lpitch);
			}else if (temp->attr&POLY4D_ATTR_SHADE_MODE_TEXTURE)
			{
				drawTextureTriangle(temp,obj->texture,lp_canvas->lp_backbuffer,lp_canvas->lpitch);
			}
		}
	}
	
};

void Canvas::lightObject(Object3d* obj){
	Light* lp_light;
	Poly* lp_poly;
	UINT r_sum = 0,g_sum = 0,b_sum = 0,r_origin = 0,g_origin = 0,b_origin = 0,r_light = 0,g_light = 0,b_light = 0;
	float dp = 0,dist = 0,atten = 0;
	Vector3d light_dir;
	if (obj->avaliable())
	{
		for (int k = 0;k<obj->num_polys;k++)
		{
			lp_poly = (obj->lp_polys) + k;
			r_sum = 0;
			g_sum = 0;
			b_sum = 0;
			if (lp_poly->attr&POLY4D_ATTR_SHADE_MODE_PURE)
			{
				continue;
			}
			else if (lp_poly->attr&POLY4D_ATTR_SHADE_MODE_FLAT)
			{
				for (std::vector<Light*>::size_type i = 0;i!=LightList.size();i++)
				{
					lp_light = LightList[i];
					if (lp_light->state == Light::LIGHTV1_STATE_OFF)
					{
						continue;
					}
					r_origin = lp_poly->color.r;
					g_origin = lp_poly->color.g;
					b_origin = lp_poly->color.b;
					switch(lp_light->attr){
					case Light::LIGHTV1_ATTR_AMBIENT:
						r_light = lp_light->c_ambient->r;
						g_light = lp_light->c_ambient->g;
						b_light = lp_light->c_ambient->b;
						r_sum += r_origin*r_light/256;
						g_sum += g_origin*g_light/256;
						b_sum += b_origin*b_light/256;
						break;
					case Light::LIGHTV1_ATTR_INFINITE:
						dp = lp_light->dir->pointMultiply(&(lp_poly->normal_vector));
						if (dp>0)
						{
							r_light = lp_light->c_diffuse->r;
							g_light = lp_light->c_diffuse->g;
							b_light = lp_light->c_diffuse->b;
							r_sum += r_origin*r_light*dp/256;
							g_sum += g_origin*g_light*dp/256;
							b_sum += b_origin*b_light*dp/256;
						}						
						break;
					case Light::LIGHTV1_ATTR_POINT:
						dist = point_distance(lp_light->pos,&((lp_poly->lp_vertex_object[lp_poly->v_index_list[0]]).pos));
						light_dir.build(&((lp_poly->lp_vertex_object[lp_poly->v_index_list[0]]).pos),lp_light->pos);
						normalizeVector3d(&light_dir);
						dp = light_dir.pointMultiply(&(lp_poly->normal_vector));
						atten = lp_light->kc + lp_light->kl*dist + lp_light->kq*dist*dist;
						if (dp>0)
						{
							r_light = lp_light->c_specular->r;
							g_light = lp_light->c_specular->g;
							b_light = lp_light->c_specular->b;
							r_sum += r_origin*r_light*dp/(256*atten*dist);
							g_sum += g_origin*g_light*dp/(256*atten*dist);
							b_sum += b_origin*b_light*dp/(256*atten*dist);
						}						
						break;
					case Light::LIGHTV1_ATTR_SPOTLIGHT_SIMPLE:
						dist = point_distance(lp_light->pos,&((lp_poly->lp_vertex_object[lp_poly->v_index_list[0]]).pos));
						light_dir.copy(lp_light->dir);
						normalizeVector3d(&light_dir);
						dp = light_dir.pointMultiply(&(lp_poly->normal_vector));
						atten = lp_light->kc + lp_light->kl*dist + lp_light->kq*dist*dist;
						if (dp>0)
						{
							r_light = lp_light->c_specular->r;
							g_light = lp_light->c_specular->g;
							b_light = lp_light->c_specular->b;
							r_sum += r_origin*r_light*dp/(256*atten*dist);
							g_sum += g_origin*g_light*dp/(256*atten*dist);
							b_sum += b_origin*b_light*dp/(256*atten*dist);
						}						
						break;
					}
				}
				lp_poly->lit_color[0].argb = _RGB32BIT(0xff,r_sum>255?255:r_sum,g_sum>255?255:g_sum,b_sum>255?255:b_sum);
			}
			else if (lp_poly->attr&POLY4D_ATTR_SHADE_MODE_GOURAUD)
			{
				for (int index = 0;index<3;index++)
				{
					r_sum = 0;
					g_sum = 0;
					b_sum = 0;
					for (std::vector<Light*>::size_type i = 0;i!=LightList.size();i++)
					{
						lp_light = LightList[i];
						if (lp_light->state == Light::LIGHTV1_STATE_OFF)
						{
							continue;
						}
						r_origin = lp_poly->color.r;
						g_origin = lp_poly->color.g;
						b_origin = lp_poly->color.b;
						switch(lp_light->attr){
						case Light::LIGHTV1_ATTR_AMBIENT:
							r_light = lp_light->c_ambient->r;
							g_light = lp_light->c_ambient->g;
							b_light = lp_light->c_ambient->b;
							r_sum += r_origin*r_light/256;
							g_sum += g_origin*g_light/256;
							b_sum += b_origin*b_light/256;
							break;
						case Light::LIGHTV1_ATTR_INFINITE:
							dp = lp_light->dir->pointMultiply(&(lp_poly->normal_vector));
							if (dp>0)
							{
								r_light = lp_light->c_diffuse->r;
								g_light = lp_light->c_diffuse->g;
								b_light = lp_light->c_diffuse->b;
								r_sum += r_origin*r_light*dp/256;
								g_sum += g_origin*g_light*dp/256;
								b_sum += b_origin*b_light*dp/256;
							}						
							break;
						case Light::LIGHTV1_ATTR_POINT:
							dist = point_distance(lp_light->pos,&((lp_poly->lp_vertex_object[lp_poly->v_index_list[0]]).pos));
							light_dir.build(&((lp_poly->lp_vertex_object[lp_poly->v_index_list[0]]).pos),lp_light->pos);
							normalizeVector3d(&light_dir);
							dp = light_dir.pointMultiply(&((lp_poly->lp_vertex_object[lp_poly->v_index_list[index]]).normal));
							atten = lp_light->kc + lp_light->kl*dist + lp_light->kq*dist*dist;
							if (dp>0)
							{
								r_light = lp_light->c_specular->r;
								g_light = lp_light->c_specular->g;
								b_light = lp_light->c_specular->b;
								r_sum += r_origin*r_light*dp/(256*atten*dist);
								g_sum += g_origin*g_light*dp/(256*atten*dist);
								b_sum += b_origin*b_light*dp/(256*atten*dist);
							}						
							break;
						case Light::LIGHTV1_ATTR_SPOTLIGHT_SIMPLE:
							dist = point_distance(lp_light->pos,&((lp_poly->lp_vertex_object[lp_poly->v_index_list[0]]).pos));
							light_dir.copy(lp_light->dir);
							normalizeVector3d(&light_dir);
							dp = light_dir.pointMultiply(&((lp_poly->lp_vertex_object[lp_poly->v_index_list[index]]).normal));
							atten = lp_light->kc + lp_light->kl*dist + lp_light->kq*dist*dist;
							if (dp>0)
							{
								r_light = lp_light->c_specular->r;
								g_light = lp_light->c_specular->g;
								b_light = lp_light->c_specular->b;
								r_sum += r_origin*r_light*dp/(256*atten*dist);
								g_sum += g_origin*g_light*dp/(256*atten*dist);
								b_sum += b_origin*b_light*dp/(256*atten*dist);
							}						
							break;
						}
					}
					lp_poly->lit_color[index].argb = _RGB32BIT(0xff,r_sum>255?255:r_sum,g_sum>255?255:g_sum,b_sum>255?255:b_sum);
				}
			}
		}

	}	
};

void Canvas::showImage(BitmapData* bitmapdata,int x,int y){
	int pos_x = x,pos_y = y;
	int sx = 0,sy = 0,ex = bitmapdata->width,ey = bitmapdata->height;
	if (x>max_clip_x||(x+bitmapdata->width)<min_clip_x||y>max_clip_y||(y+bitmapdata->height)<min_clip_y)
	{
		return;
	}
	if (x<min_clip_x)
	{
		sx = min_clip_x - x;
		pos_x = 0;
	}
	if (y<min_clip_y)
	{
		sy = min_clip_y - y;
		pos_y = 0;
	}
	if ((bitmapdata->width + pos_x)>max_clip_x)
	{
		ex = 2*bitmapdata->width - max_clip_x - pos_x;
	}
	if ((bitmapdata->height + pos_y)>max_clip_y)
	{
		ey = 2*bitmapdata->height - max_clip_y - pos_y;
	}
	if (bitmapdata->power_of_two != -1)
	{
		for (int i = sy;i<ey;i++)
		{
			for (int j = sx;j<ex;j++)
			{
				plotPixel(pos_x + j,pos_y + i,bitmapdata->data[(i<<bitmapdata->power_of_two) + j].argb);
			}
		}
	}
	else{
		for (int i = sy;i<ey;i++)
		{
			for (int j = sx;j<ex;j++)
			{
				plotPixel(pos_x + j,pos_y + i,bitmapdata->data[i*bitmapdata->width + ex].argb);
			}
		}
	}
};


Canvas::~Canvas(void)
{
	releaseT(lp_primary_surface);
	if (is_window)
	{
		releaseT(lp_back_surface);
	}	
	releaseT(lp_directdraw);
	delete lp_camera;
};
