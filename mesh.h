#ifndef __MESH_H
#define __MESH_H

#include <iostream>
#include <stdio.h> //file functions
#include <string>
#include <vector>
#include "vector3f.h"

#define X 0
#define Y 1
#define Z 2

using namespace std;

typedef struct rgb_col {
	float r, g, b;
} Color;

typedef struct tri {
	vector3f v0, v1, v2;
	vector3f normals[3];	//vertices normals
	vector3f t_normal;		//triangle normal
	Color color;
} Triangle;

class Mesh {
	public:
		//atributes
		string name;
		int n_triangles;
		
		vector<Triangle> triangles;
		float max[3];	//x, y and z max values
		float min[3]; 	//x, y and z min values
				
		//constructors & destructors
		Mesh();
		Mesh(string name);
		~Mesh();
		
		//methods
		void readFromFile();
		void print();
};
#endif
