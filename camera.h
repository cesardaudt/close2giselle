#ifndef __CAMERA_H
#define __CAMERA_H

#include "vector3f.h"
#include <math.h>
#include "matrix4x4f.h"
#include <iostream>
#include <stdlib.h>
#include <GL/glui.h>
#include <GL/glut.h>

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
