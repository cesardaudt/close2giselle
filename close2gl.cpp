#include "close2gl.h"

close2gl::close2gl(int width, int height, int win_x, int win_y, int* bfculling, int* vertex_orientation, Camera* cam, Mesh* mesh) {
	this->width				  = width;
	this->height		 	  = height;
	this->win_x				  = win_x;
	this->win_y				  = win_y;
	this->bfculling			  = bfculling;
	this->vertex_orientation  = vertex_orientation;
	this->n_clipped_triangles = 0;
	this->cam				  = cam;
	this->mesh				  = mesh;
	
	this->framebuffer		  = (unsigned char*) malloc(CHANNELS * width * height * sizeof(unsigned char));
	
	memset(this->framebuffer, 0, CHANNELS * width * height);
	//Now, we have the triangle mesh and u,v,n vectors
	//To display the image, we should perform:
	//1)Coordinates mapping (WCS->CCS and CCS->SCS)
	//2)Clipping
	//3)Perspective Division
	//4)Viewport mapping
	//5)Draw
}

close2gl::close2gl() {
	width = 1;
	height= 1;
	win_x = 1;
	win_y = 1;
	n_clipped_triangles = 0;
	bfculling = NULL;
	cam   = NULL;
	mesh  = NULL;
	framebuffer = NULL;
}

close2gl::~close2gl() {}
		
matrix4x4f close2gl::modelView() {
//	⎡ xc ⎤ ⎡ux uy uz − Oc ⋅ u ⎤ ⎡ xw ⎤
//	⎢ yc ⎥=⎢vx vy vz − Oc ⋅ v ⎥ ⎢ yw ⎥
//	⎢ zc ⎥ ⎢nx ny nz − Oc ⋅ n ⎥ ⎢ zw ⎥
//	⎣ 1  ⎦ ⎣0  0  0    1      ⎦ ⎣ 1  ⎦
	
//TODO: row of z component with sign inverted
// hardcoded minus to make it work
	matrix4x4f m;
	m.m[0] = cam->u.x   ; m.m[4] = cam->u.y   ; m.m[8]  = cam->u.z   ; m.m[12] = vector3f::dotProduct(-(cam->look_from), cam->u);
	m.m[1] = cam->v.x   ; m.m[5] = cam->v.y   ; m.m[9]  = cam->v.z   ; m.m[13] = vector3f::dotProduct(-(cam->look_from), cam->v);
	m.m[2] = -(cam->n.x); m.m[6] = -(cam->n.y); m.m[10] = -(cam->n.z); m.m[14] =(vector3f::dotProduct(-(cam->look_from), -(cam->n)));
	m.m[3] = 0          ; m.m[7] =  0         ; m.m[11] = 0          ; m.m[15] = 1;
	return m;
	
}

matrix4x4f close2gl::projection() {
//⎡  2n               (r + l)        ⎤
//⎢(r − l )    0      (r − l)    0   ⎥
//⎢                                  ⎥
//⎢           2n      (t + b)        ⎥
//⎢   0     (t − b)   (t − b)    0   ⎥
//⎢                                  ⎥
//⎢                  -(f + n)  -2fn  ⎥
//⎢   0        0      (f − n) (f − n)⎥
//⎢                                  ⎥
//⎣   0        0      -1         0   ⎦

	float r, l, t, b, n, f;
	t = cam->znear * tan(DEG_RAD(cam->fovy/2));
	b = -t;
	r = t * ((float)width/(float)height);
	l = -r;
	n = cam->znear;
	f = cam->zfar;
	
	matrix4x4f m;
	m.m[0] = (2*n)/(r-l); m.m[4] = 0          ; m.m[8]  = (r+l)/(r-l); m.m[12] = 0;
	m.m[1] = 0          ; m.m[5] = (2*n)/(t-b); m.m[9]  = (t+b)/(t-b); m.m[13] = 0;
	m.m[2] = 0          ; m.m[6] = 0          ; m.m[10] =-(f+n)/(f-n); m.m[14] = -(2*f*n)/(f-n);
	m.m[3] = 0          ; m.m[7] = 0          ; m.m[11] =-1          ; m.m[15] = 0;
	return m;
}

matrix4x4f close2gl::viewport() {
//⎡ (rv − lv)    0          0   (rv + lv) ⎤
//⎢     2                           2     ⎥
//⎢          (tv − bv)      0   (tv + bv) ⎥
//⎢   0          2                  2     ⎥
//⎢                                       ⎥
//⎢  0          0           1       0     ⎥
//⎢                                       ⎥
//⎢   0         0           0       1     ⎥
//⎣                                       ⎦

	float rv, lv, tv, bv;
	lv = win_x;
	bv = win_y;
	rv = win_x+width;
	tv = win_y+height;

	matrix4x4f m;
	m.m[0] = (rv-lv)/2	; m.m[4] = 0          ; m.m[8]  = 0 ; m.m[12] = (rv+lv)/2;
	m.m[1] = 0          ; m.m[5] = (tv-bv)/2  ; m.m[9]  = 0 ; m.m[13] = (tv+bv)/2;
	m.m[2] = 0          ; m.m[6] = 0          ; m.m[10] = 1	; m.m[14] = 0;
	m.m[3] = 0          ; m.m[7] = 0          ; m.m[11] = 0 ; m.m[15] = 1;
	return m;
}

void close2gl::mainLoop() {
	HomTri aux;
	matrix4x4f proj;
	matrix4x4f modview;
	matrix4x4f vport;
	matrix4x4f mproj;

	//Map WCS -> CCS -> SCS		
	proj 	= projection();
	modview = modelView();
	vport	= viewport();

	//foreach triangle, do Pi_scs = Projection * Modelview * Pi_wcs
	for(int i=0; i<mesh->n_triangles; i++) {
		to4d(aux,mesh->triangles[i]);
		//Pi_m = ModelView * Pi_wcs
		modview.transform(&(aux.v0));
		modview.transform(&(aux.v1));
		modview.transform(&(aux.v2));
				
		//Pi_scs = Projection * Pi_m
		proj.transform(&(aux.v0));
		proj.transform(&(aux.v1));
		proj.transform(&(aux.v2));
		triangles.push_back(aux);
	}
	
	//Clipping
	//foreach triangle, if one point doesn't satisfy fabs(x), fabs(y), fabs(z) <= fabs(w), do not draw it
	for(int i=0; i<mesh->n_triangles; i++) {
			 //vertex v0
		if(!((fabs(triangles[i].v0.vec.x) <= fabs(triangles[i].v0.w)) &&
			 (fabs(triangles[i].v0.vec.y) <= fabs(triangles[i].v0.w)) &&
			 (fabs(triangles[i].v0.vec.z) <= fabs(triangles[i].v0.w)) &&
			 //vertex v1
			 (fabs(triangles[i].v1.vec.x) <= fabs(triangles[i].v1.w)) &&
			 (fabs(triangles[i].v1.vec.y) <= fabs(triangles[i].v1.w)) &&
			 (fabs(triangles[i].v1.vec.z) <= fabs(triangles[i].v1.w)) &&
			 //vertex v2
			 (fabs(triangles[i].v2.vec.x) <= fabs(triangles[i].v2.w)) &&
			 (fabs(triangles[i].v2.vec.y) <= fabs(triangles[i].v2.w)) &&
			 (fabs(triangles[i].v2.vec.z) <= fabs(triangles[i].v2.w))	))
		{
			triangles[i].draw = false;
		}
	}
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0, width, 0, height);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
		
	//Backface culling
	if(*bfculling != 0) {
	float dot_result;
		for(int i = 0; i < mesh->n_triangles; i++) {
			//if the triangle passed the clipping stage, we should perform culling
			if(triangles[i].draw == true){
				triangles[i].t_normal = 
					vector3f::crossProduct(triangles[i].v1.vec - triangles[i].v0.vec,
										   triangles[i].v2.vec - triangles[i].v0.vec);
				
				dot_result = vector3f::dotProduct(triangles[i].t_normal, cam->n);
				
				//CW case			
				if(*vertex_orientation != 0) {
					if(dot_result <= 0) {
						triangles[i].draw = false;
					}
				}
				//CCW case
				else {
					if(dot_result >= 0) {
						triangles[i].draw = false;
					}
				}
			}
		}
	}
	
	//Perspective division	
	for(int i = 0; i < mesh->n_triangles; i++) {
		if (triangles[i].draw == true) {
			//v0
			triangles[i].v0.vec = (triangles[i].v0.vec * (1.0f / triangles[i].v0.w));
			triangles[i].v0.w   = 1;
			//v1
			triangles[i].v1.vec = (triangles[i].v1.vec * (1.0f / triangles[i].v1.w));
			triangles[i].v1.w   = 1;
			//v2
			triangles[i].v2.vec = (triangles[i].v2.vec * (1.0f / triangles[i].v2.w));
			triangles[i].v2.w   = 1;	
		}
	}
	
	//Maps to viewport
	for(int i = 0; i < mesh->n_triangles; i++) {
		if (triangles[i].draw == true) {
			vport.transform(&(triangles[i].v0));
			vport.transform(&(triangles[i].v1));
			vport.transform(&(triangles[i].v2));
		}
	}

	//Draw
	//done at display func

	system("clear");
}

void close2gl::swap(int& a, int& b) {
	int aux = a;
	a = b;
	b = aux;
}

void close2gl::plot(int x, int y) {
	Color color = {255, 0, 0};
	setPixel(framebuffer, width, height, CHANNELS, x, y, color);
}

void close2gl::bresenhamLine(int x0, int x1, int y0, int y1) {
	int deltax, deltay, ystep, y, x;
	double error, deltaerror;
	bool steep;
	steep = (abs(y1 - y0) > abs(x1 - x0));
	
	if(steep) {
		swap(x0, y0);
		swap(x1, y1);
	}
	if(x0 > x1) {
		swap(x0, x1);
		swap(y0, y1);
	}
	
	deltax = x1-x0;
	deltay = abs(y1-y0);
	error  = 0;
	deltaerror = (double)deltay/(double)deltax;
	y = y0;
	
	if(y0 < y1) {
		ystep = 1;
	}
	else {
		ystep = -1;
	}
	
	for(x = x0; x<=x1; x++) {
		if(steep) {
			plot(y,x);
		}
		else {
			plot(x,y);
		}
		
		error = error+deltaerror;

		if(error >= 0.5) {
			y = y+ystep;
			error = error - 1.0;
		}
	}
}

//debug
void close2gl::printVec(vector4f v) {
	printf("<%f,%f,%f,%f>\n", v.vec.x, v.vec.y, v.vec.z, v.w);
}

void close2gl::printMatrix(matrix4x4f mat) {
	for(int i = 0; i<4; i++) {
		printf("%f \t %f \t %f \t %f \n", mat.m[i%4], mat.m[(i%4)+4], mat.m[(i%4)+8], mat.m[(i%4)+12]);
	}
}
