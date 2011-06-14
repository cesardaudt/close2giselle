#ifndef __CLOSE2GL_H
#define __CLOSE2GL_H

#include "camera.h"
#include "matrix4x4f.h"
#include "mesh.h"
#include <string>
#include <vector>

#define to4d(tri4d, tri3d)  tri4d.v0.vec	= tri3d.v0; tri4d.v0.w = 1; \
							tri4d.v1.vec	= tri3d.v1; tri4d.v1.w = 1; \
							tri4d.v2.vec	= tri3d.v2; tri4d.v2.w = 1; \
							tri4d.normals[0]= tri3d.normals[0]; \
							tri4d.normals[1]= tri3d.normals[1]; \
							tri4d.normals[2]= tri3d.normals[2]; \
							tri4d.t_normal	= tri3d.t_normal; \
							tri4d.color		= tri3d.color; \
							tri4d.draw		= true

#define returnNormal(tri)	((vector3f::crossProduct(vector3f(tri.v1.vec - tri.v0.vec), vector3f(tri.v2.vec - tri.v0.vec))))

class close2gl {
	public:
		//atributes
		int				width;
		int				height;
		int				win_x;
		int				win_y;
		int				n_clipped_triangles;
		int*			bfculling;			//pointer to global var bfculling
		int*			vertex_orientation;
		Camera*			cam;
		Mesh* 			mesh;
		vector<HomTri>  triangles;
		//constructors & destructors
		close2gl();
		close2gl(int width, int height, int win_x, int win_y, int* bfculling, int* vertex_orientation, Camera* cam, Mesh* mesh);
		~close2gl();
		
		//methods
		//returns current ModelView matrix
		matrix4x4f modelView();
		//returns current Projection matrix
		matrix4x4f projection();
		//returns current Viewport matrix
		matrix4x4f viewport();
		
		//Map WCS -> CCS -> SCS
		//Clipping
		//Perpective division
		//Map to viewport
		//Draw
		void mainLoop();
		
		//debug
		void printMatrix(matrix4x4f mat);
		void printVec(vector4f v);
};

#endif
