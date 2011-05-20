#include "close2gl.h"

close2gl::close2gl(string m_name, int w, int h, float near, float far): mesh(m_name),
																		cam(mesh.max, mesh.min, w, h, vector3f(0,0,0), vector3f(0,0,0), false) 
{
	mesh_name = m_name;
	width = w;
	height = h;
	cam.znear = near;
	cam.zfar = far;
	//Now, we have the triangle mesh and u,v,n vectors
	//To display the image, we should perform:
	//1)Coordinates mapping (WCS->CCS and CCS->SCS)
	//2)Clipping
	//3)Perspective Division
	//4)Viewport mapping
	//5)Draw
}

close2gl::close2gl() {

}

close2gl::~close2gl() {

}
		
matrix4x4f close2gl::modelView() {
//	⎡ xc ⎤ ⎡ux uy uz − Oc ⋅ u ⎤ ⎡ xw ⎤
//	⎢ yc ⎥=⎢vx vy vz − Oc ⋅ v ⎥ ⎢ yw ⎥
//	⎢ zc ⎥ ⎢nx ny nz − Oc ⋅ n ⎥ ⎢ zw ⎥
//	⎣ 1  ⎦ ⎣0  0  0    1      ⎦ ⎣ 1  ⎦
	
	matrix4x4f m;
	m.m[0] = cam.u.x; m.m[1] = cam.u.y; m.m[2]  = cam.u.z; m.m[3]  = vector3f::dotProduct(-(cam.look_from), cam.u);
	m.m[4] = cam.v.x; m.m[5] = cam.v.y; m.m[6]  = cam.v.z; m.m[7]  = vector3f::dotProduct(-(cam.look_from), cam.v);
	m.m[8] = cam.n.x; m.m[9] = cam.n.y; m.m[10] = cam.n.z; m.m[11] = vector3f::dotProduct(-(cam.look_from), cam.n);
	m.m[12]= 0      ; m.m[13]= 0      ; m.m[14] = 0      ; m.m[15] = 1;
	return m;
}

matrix4x4f close2gl::projection() {
//⎡  2n               (r + l)        ⎤
//⎢(r − l )    0      (r − l)    0   ⎥
//⎢                                  ⎥
//⎢			2n      (t + b)          ⎥
//⎢   0	  (t − b)   (t − b)    0     ⎥
//⎢                                  ⎥
//⎢                  -(f + n)  -2fn  ⎥
//⎢   0        0      (f − n) (f − n)⎥
//⎢                                  ⎥
//⎣   0        0      -1         0   ⎦

	float r, l, t, b, n, f;
	r = mesh.max[0];
	l = mesh.min[0];
	t = mesh.max[1];
	b = mesh.min[1];
	n = cam.znear;
	f = cam.zfar;

	matrix4x4f m;
	m.m[0] = (2*n)/(r-l); m.m[1] = 0          ; m.m[2]  = (r+l)/(r-l); m.m[3] = 0;
	m.m[4] = 0          ; m.m[5] = (2*n)/(t-b); m.m[6]  = (t+b)/(t-b); m.m[7] = 0;
	m.m[8] = 0          ; m.m[9] = 0          ; m.m[10] =-(f+n)/(f-n); m.m[11]= -(2*f*n)/(f-n);
	m.m[12]= 0          ; m.m[13]= 0          ; m.m[14] =-1          ; m.m[15]= 0;
	return m;


}

matrix4x4f close2gl::viewport() {

}
