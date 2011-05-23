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
							tri4d.color		= tri3d.color

class close2gl {
	public:
		//atributes
		int				width;
		int				height;
		int				win_x;
		int				win_y;
		int				n_clipped_triangles;
		Camera*			cam;
//		matrix4x4f 		transform;	//maybe it's not necessary
//		matrix4x4f 		aux;		//maybe it's not necessary
		Mesh* 			mesh;
//		string			mesh_name;	//maybe it's not necessary
		vector<HomTri>	SCStriangles;
		vector<HomTri>	clipped_triangles;
		//constructors & destructors
		close2gl();
		close2gl(int width, int height, int win_x, int win_y, Camera* cam, Mesh* mesh);
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
};

#endif
