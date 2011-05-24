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

Camera::~Camera() {}

void Camera::set(float max[3], float min[3], int w, int h, vector3f translate, vector3f rotation, int lookat) {
	float hy=0, hx=0, ph=0;
	float tx=0, ty=0, tz=0;

	look_from.x = look_at.x = (max[0] + min[0])/2; 
	look_from.y = look_at.y = (max[1] + min[1])/2;
	
	//TODO: Should work if fovy xor fovx is given
	//		Should be clearer
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

	look_from += translate;	
		
	if(!lookat) {
		look_at += translate;
	}

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
	rx.transformVector(&n);
	rx.transformVector(&v);
		
	//rotation in y
	matrix4x4f ry;
	ry.rotate(rotation.y, v);
	ry.transformVector(&n);
	ry.transformVector(&u);

	//rotation in z
	matrix4x4f rz;
	rz.rotate(rotation.z, n);
	rz.transformVector(&v);
	rz.transformVector(&u);

    gluLookAt(look_from.x      , look_from.y      , look_from.z      ,
			  look_from.x + n.x, look_from.y + n.y, look_from.z + n.z,
			  v.x		       , v.y	          , v.z              );
	
	//debug
//    float mat2[4][4];
//	glGetFloatv(GL_MODELVIEW_MATRIX, (float*)mat2);
//	printf("OpenGL modelview:\n");
//	for(int i = 0; i<4; i++) {
//		for(int j = 0; j<4; j++) {
//			printf("%f \t", mat2[j][i]);
//		}
//		printf("\n");
//	}
////	
//	float mat[4][4];
//	glGetFloatv(GL_PROJECTION_MATRIX, (float*)mat);
//	printf("OpenGL projection:\n");
//	for(int i = 0; i<4; i++) {
//		for(int j = 0; j<4; j++) {
//			printf("%f \t", mat[j][i]);
//		}
//		printf("\n");
//	}
//  float mat3[4][4];
//	glGetFloatv(GL_VIEWPORT, (float*)mat3);
//	printf("OpenGL viewport:\n");
//	for(int i = 0; i<4; i++) {
//		for(int j = 0; j<4; j++) {
//			printf("%f \t", mat3[j][i]);
//		}
//		printf("\n");
//	}
}
