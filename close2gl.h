#ifndef __CLOSE2GL_H
#define __CLOSE2GL_H

#include "camera.h"
#include "matrix4x4f.h"
#include "mesh.h"
#include <string>
#include <vector>

class close2gl {
	public:
		//atributes
		int				width;
		int				height;
		int				win_x;
		int				win_y;
		Camera 			cam;
		matrix4x4f 		transform;
		matrix4x4f 		aux;		//maybe it's not necessary
		Mesh 			mesh;
		string			mesh_name;
		vector<vec4d>	SCSTriangles;
		
		//constructors & destructors
		close2gl();
		close2gl(string m_name, int w, int h, float near, float far);
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
		void mainLoop(Camera cam);
};

#endif
