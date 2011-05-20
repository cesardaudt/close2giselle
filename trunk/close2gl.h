#ifndef __CLOSE2GL_H
#define __CLOSE2GL_H

#include "camera.h"
#include "matrix4x4f.h"
#include "mesh.h"
#include <string>

class close2gl {
	public:
		//atributes
		int			width;
		int			height;
		int			win_x;
		int			win_y;
		Camera 		cam;
		matrix4x4f 	transform;
		matrix4x4f 	aux;		//maybe it's not necessary
		Mesh 		mesh;
		string		mesh_name;
			
		//constructors & destructors
		close2gl();
		close2gl(string m_name, int w, int h, float near, float far);
		~close2gl();
		
		//methods
		matrix4x4f modelView();
		matrix4x4f projection();
		matrix4x4f viewport();		
};

#endif
