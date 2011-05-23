#ifndef __CAMERA_H
#define __CAMERA_H

#include "vector3f.h"
#include <math.h>
#include "matrix4x4f.h"
#include <iostream>
#include <stdlib.h>
#include <GL/glui.h>
#include <GL/glut.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#define DEG_RAD(theta) (theta*M_PI)/180
#define maxabs(x,y) ((abs(x)>abs(y)) ? (x) : (y))

class Camera {
	public:
		//atributes
		vector3f u, v, n;
		vector3f look_at;
		vector3f look_from;
		float fovy;
		float fovx;
		float znear;
		float zfar;
			
		//constructors & destructors
		Camera();
		~Camera();
		
		//methods
		void set(float max[3], float min[3], int w, int h, vector3f translate, vector3f rotation, int lookat);
};

#endif
