#include "camera.h"

Camera::Camera() {
	u.x = 1; u.y = 0; u.z = 0;
	v.x = 0; v.y = 1; v.z = 0;
	n.x = 0; n.y = 0; n.z = 1;
	
	znear = 0.1;
	zfar  = 3000;
	
	look_at.x = look_at.y = look_at.z = 0;
	look_from.x = look_at.x + n.x; 
	look_from.y = look_at.y + n.y;
	look_from.z = look_at.z - n.z;
	
	fovy = 90;
	fovx = 0;	//to be computed later
}

Camera::~Camera() {

}

void Camera::set(float max[2], float min[2], int w, int h, vector3f translate, vector3f rotation, int lookat) {
	float hy=0, hx=0, ph=0;
	float tx=0, ty=0, tz=0;

	look_from.x = look_at.x = (max[0] + min[0])/2; 
	look_from.y = look_at.y = (max[1] + min[1])/2;
	
	//rewrite this part
	hy = ((max[1] - min[1])/2)/tan(DEG_RAD(fovy/2.0));

	ph = (h/2)/tan(DEG_RAD(fovy/2));
	fovx = atan((w/2)/ph)*2.0;
	hx = ((max[0] - min[0])/2)/tan(fovx/2.0);

	look_from.z = maxabs(hx,hy)+max[2];
	look_at.z = -look_from.z;	
	//
	
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	
	gluPerspective(fovy, (float)w/(float)h, znear, zfar);
    
    glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

//	look_from.x += translate.x;
//	look_from.y += translate.y;
//	look_from.z += translate.z;
	look_from += translate;	
		
	if(!lookat) {
//		look_at.x += translate.x;
//		look_at.y += translate.y;
//		look_at.z += translate.z;
		look_at += translate;
	}

//	n.x = -look_from.x + look_at.x;
//	n.y = -look_from.y + look_at.y;
//	n.z = look_at.z;
	n = look_at - look_from;
	
	n.normalize();
	
	//n_aux is in the plane defined by n and v
	vector3f n_aux(n.x, n.y-1, n.z);
	u = vector3f::crossProduct(n_aux,n);
	u.normalize();
	v = vector3f::crossProduct(u,n);
	v.normalize();
	
	//rotation in x
	matrix4x4f rx;
	rx.rotate(rotation.x, u);
	
//	vector3f aux_n;
//	aux_n.x = n.x; aux_n.y = n.y; aux_n.z = n.z;
//	rx.transformVector(&aux_n);
//	n.x = aux_n.x; n.y = aux_n.y; n.z = aux_n.z;
	rx.transformVector(&n);

	
//	vector3f aux_v;
//	aux_v.x = v.x; aux_v.y = v.y; aux_v.z = v.z;
//	rx.transformVector(&aux_v);
//	v.x = aux_v.x; v.y = aux_v.y; v.z = aux_v.z;
	rx.transformVector(&v);
		
	//rotation in y
	matrix4x4f ry;
	ry.rotate(rotation.y, v);
	
//	vector3f aux_n;
//	aux_n.x = n.x; aux_n.y = n.y; aux_n.z = n.z;
//	ry.transformVector(&aux_n);
//	n.x = aux_n.x; n.y = aux_n.y; n.z = aux_n.z;
	ry.transformVector(&n);

	
//	vector3f aux_u;
//	aux_u.x = u.x; aux_u.y = u.y; aux_u.z = u.z;
//	ry.transformVector(&aux_u);
//	u.x = aux_u.x; u.y = aux_u.y; u.z = aux_u.z;
	ry.transformVector(&u);

	//rotation in z
	matrix4x4f rz;
	rz.rotate(rotation.z, n);

//	vector3f aux_v;
//	aux_v.x = v.x; aux_v.y = v.y; aux_v.z = v.z;
//	rz.transformVector(&aux_v);	
//	v.x = aux_v.x; v.y = aux_v.y; v.z = aux_v.z;
	rz.transformVector(&v);
	
//	vector3f aux_u;
//	aux_u.x = u.x; aux_u.y = u.y; aux_u.z = u.z;
//	rz.transformVector(&aux_u);
//	u.x = aux_u.x; u.y = aux_u.y; u.z = aux_u.z;
	rz.transformVector(&u);

    gluLookAt(look_from.x      , look_from.y      , look_from.z      ,
			  look_from.x + n.x, look_from.y + n.y, look_from.z + n.z,
			  v.x		       , v.y	          , v.z              );

}
