#include "close2gl.h"

close2gl::close2gl(int width, int height, int win_x, int win_y, Camera* cam, Mesh* mesh) {
	this->width				  = width;
	this->height		 	  = height;
	this->win_x				  = win_x;
	this->win_y				  = win_y;
	this->n_clipped_triangles = 0;
	this->cam				  = cam;
	this->mesh				  = mesh;
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
	cam   = NULL;
	mesh  = NULL;
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
	m.m[0] = cam->u.x; m.m[4] = cam->u.y; m.m[8]  = cam->u.z; m.m[12] = vector3f::dotProduct(-(cam->look_from), cam->u);
	m.m[1] = cam->v.x; m.m[5] = cam->v.y; m.m[9]  = cam->v.z; m.m[13] = vector3f::dotProduct(-(cam->look_from), cam->v);
	m.m[2] =-(cam->n.x); m.m[6] =-(cam->n.y); m.m[10] =-(cam->n.z); m.m[14] = -(vector3f::dotProduct(-(cam->look_from), cam->n));
	m.m[3] = 0       ; m.m[7]= 0        ; m.m[11] = 0       ; m.m[15] = 1;
	return m;
}

matrix4x4f close2gl::projection() {
//⎡  2n               (r + l)        ⎤
//⎢(r − l )    0      (r − l)    0   ⎥
//⎢                                  ⎥
//⎢			  2n      (t + b)        ⎥
//⎢   0	    (t − b)   (t − b)    0   ⎥
//⎢                                  ⎥
//⎢                  -(f + n)  -2fn  ⎥
//⎢   0        0      (f − n) (f − n)⎥
//⎢                                  ⎥
//⎣   0        0      -1         0   ⎦

	float r, l, t, b, n, f;
	t = cam->znear * tan(cam->fovy/2);
	b = -t;
	r = t * (width/height);
	l = -r;
	
//	printf("\nznear:%f fovy:%f\n", cam->znear, cam->fovy);
	printf("\nwidth:%f height:%f\n", width, height);
	printf("\nt:%f b:%f r:%f l:%f\n", t,b,r,l);
	
	matrix4x4f m;
	m.m[0] = (2*n)/(r-l); m.m[4] = 0          ; m.m[8]  = (r+l)/(r-l); m.m[12] = 0;
	m.m[1] = 0          ; m.m[5] = (2*n)/(t-b); m.m[9]  = (t+b)/(t-b); m.m[13] = 0;
	m.m[2] = 0          ; m.m[6] = 0          ; m.m[10] =-(f+n)/(f-n); m.m[14] = -(2*f*n)/(f-n);
	m.m[3] = 0          ; m.m[7] = 0          ; m.m[11] =-1          ; m.m[15] = 0;
	return m;
}

matrix4x4f close2gl::viewport() {
//⎡ (rv − lv)	0			0	(rv + lv) ⎤
//⎢	    2							2	  ⎥
//⎢	  		 (tv − bv)		0	(tv + bv) ⎥
//⎢	  0			 2					2     ⎥
//⎢										  ⎥
//⎢	  0			0			1		0     ⎥
//⎢										  ⎥
//⎢   0			0			0		1	  ⎥
//⎣										  ⎦

	float rv, lv, tv, bv;
	lv = 0+width;
	bv = 0+height;
	rv = lv+width;
	tv = bv-height;

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
	printf("\n My Projection matrix\n");
	printMatrix(proj);
	
	modview = modelView();
	printf("\n My ModelView matrix\n");
	printMatrix(modview);
	
	mproj 	= proj*modview;
	
	vport	= viewport();
//	printf("\nViewport matrix\n");
//	printMatrix(vport);
	
	SCStriangles.reserve(mesh->n_triangles);
	//foreach triangle, do Pi_scs = Projection * Modelview * Pi_wcs
	for(int i=0; i<mesh->n_triangles; i++) {
		to4d(aux,mesh->triangles[i]);
		mproj.transform(&(aux.v0));
		mproj.transform(&(aux.v1));
		mproj.transform(&(aux.v2));
		SCStriangles[i] = aux;
	}
	
	//Clipping
	//foreach triangle, if one point doesn't satisfy abs(x), abs(y), abs(z) < abs(w), do not draw it
	n_clipped_triangles = 0;
	for(int i=0; i<mesh->n_triangles; i++) {
			//vertex v0
		if(	(abs(SCStriangles[i].v0.vec.x) < abs(SCStriangles[i].v0.w)) &&
			(abs(SCStriangles[i].v0.vec.y) < abs(SCStriangles[i].v0.w)) &&
			(abs(SCStriangles[i].v0.vec.z) < abs(SCStriangles[i].v0.w)) &&
			//vertex v1
			(abs(SCStriangles[i].v1.vec.x) < abs(SCStriangles[i].v1.w)) &&
			(abs(SCStriangles[i].v1.vec.y) < abs(SCStriangles[i].v1.w)) &&
			(abs(SCStriangles[i].v1.vec.z) < abs(SCStriangles[i].v1.w)) &&
			//vertex v2
			(abs(SCStriangles[i].v2.vec.x) < abs(SCStriangles[i].v2.w)) &&
			(abs(SCStriangles[i].v2.vec.y) < abs(SCStriangles[i].v2.w)) &&
			(abs(SCStriangles[i].v2.vec.z) < abs(SCStriangles[i].v2.w))		)
		{
			clipped_triangles.push_back(SCStriangles[i]);
			n_clipped_triangles++;
		}
	}
	
	//Perspective division
	for(int i = 0; i < n_clipped_triangles; i++) {
		clipped_triangles[i].v0.vec.x /= clipped_triangles[i].v0.w;
		clipped_triangles[i].v0.vec.y /= clipped_triangles[i].v0.w;
		clipped_triangles[i].v0.vec.z /= clipped_triangles[i].v0.w;
		clipped_triangles[i].v0.w  = 1;
	
		clipped_triangles[i].v1.vec.x /= clipped_triangles[i].v1.w;
		clipped_triangles[i].v1.vec.y /= clipped_triangles[i].v1.w;
		clipped_triangles[i].v1.vec.z /= clipped_triangles[i].v1.w;
		clipped_triangles[i].v1.w  = 1;
		
		clipped_triangles[i].v2.vec.x /= clipped_triangles[i].v2.w;
		clipped_triangles[i].v2.vec.y /= clipped_triangles[i].v2.w;
		clipped_triangles[i].v2.vec.z /= clipped_triangles[i].v2.w;
		clipped_triangles[i].v2.w  = 1;	
	}
	
	//Maps to viewport
	for(int i = 0; i < n_clipped_triangles; i++) {
		vport.transform(&(clipped_triangles[i].v0));
		vport.transform(&(clipped_triangles[i].v1));
		vport.transform(&(clipped_triangles[i].v2));
	}
	
	//Draw
	//done at display func
	system("clear");
}

void close2gl::printMatrix(matrix4x4f mat) {
	for(int i = 0; i<4; i++) {
		printf("%f \t %f \t %f \t %f \n", mat.m[i%4], mat.m[(i%4)+4], mat.m[(i%4)+8], mat.m[(i%4)+12]);
	}
}
