#include "mesh.h"

Mesh::Mesh() {
	name = string("");
	n_triangles = 0;
	max[0] = max[1] = max[2] = -99999;
	min[0] = min[1] = min[2] =  99999;
}

Mesh::Mesh(string name) {
	this->name = name;
	readFromFile();
}

Mesh::~Mesh() {

}

void Mesh::readFromFile() {
	vector<Color> ambient;
	vector<Color> diffuse;
	vector<Color> specular;
	vector<float> material_shine;
	
	char ch;
	int i, material_count, color_index[3];
	
	FILE* f_pt = fopen(name.c_str(), "r");
	
	if(f_pt == NULL) {
		cout << "Error opening file " << name << endl;
	}
	else {
		fscanf(f_pt, "%c", &ch);
		//skips model's name
		while(ch != '\n')
			fscanf(f_pt, "%c", &ch);
		
		fscanf(f_pt, "# triangles = %d\n", &n_triangles);	//read no. of triangles
		fscanf(f_pt, "Material count = %d\n", &material_count);
		
		ambient.reserve(material_count);
		diffuse.reserve(material_count);
		specular.reserve(material_count);
		material_shine.reserve(material_count);
		for(i=0; i<material_count; i++) {
			fscanf(f_pt, "ambient color %f %f %f\n", &(ambient[i].r), &(ambient[i].g), &(ambient[i].b));
			fscanf(f_pt, "diffuse color %f %f %f\n", &(diffuse[i].r), &(diffuse[i].g), &(diffuse[i].b));
			fscanf(f_pt, "specular color %f %f %f\n",&(specular[i].r),&(specular[i].g),&(specular[i].b));
			fscanf(f_pt, "material shine %f\n", &(material_shine[i]));
		}
		
		fscanf(f_pt, "%c", &ch);
		//skips documentation line
		while(ch != '\n')
			fscanf(f_pt, "%c", &ch);
		
		triangles.reserve(n_triangles); //alocate space
		//read triangles
		//TODO: try to find max and min values with vector functions
		for(i=0; i<n_triangles; i++) {
			fscanf(f_pt, "v0 %f %f %f %f %f %f %d\n",
					&(triangles[i].v0.x), &(triangles[i].v0.y), &(triangles[i].v0.z),
					&(triangles[i].normals[0].x), &(triangles[i].normals[0].y), &(triangles[i].normals[0].z),
					&(color_index[0]));
			if(max[X] < (triangles[i].v0.x)) max[X] = (triangles[i].v0.x);
			if(max[Y] < (triangles[i].v0.y)) max[Y] = (triangles[i].v0.y);
			if(max[Z] < (triangles[i].v0.z)) max[Z] = (triangles[i].v0.z);
			if(min[X] > (triangles[i].v0.x)) min[X] = (triangles[i].v0.x);
			if(min[Y] > (triangles[i].v0.y)) min[Y] = (triangles[i].v0.y);
			if(min[Z] > (triangles[i].v0.z)) min[Z] = (triangles[i].v0.z);
			
			fscanf(f_pt, "v1 %f %f %f %f %f %f %d\n",
					&(triangles[i].v1.x), &(triangles[i].v1.y), &(triangles[i].v1.z),
					&(triangles[i].normals[1].x), &(triangles[i].normals[1].y), &(triangles[i].normals[1].z),
					&(color_index[1]));
			if(max[X] < (triangles[i].v1.x)) max[X] = (triangles[i].v1.x);
			if(max[Y] < (triangles[i].v1.y)) max[Y] = (triangles[i].v1.y);
			if(max[Z] < (triangles[i].v1.z)) max[Z] = (triangles[i].v1.z);
			if(min[X] > (triangles[i].v1.x)) min[X] = (triangles[i].v1.x);
			if(min[Y] > (triangles[i].v1.y)) min[Y] = (triangles[i].v1.y);
			if(min[Z] > (triangles[i].v1.z)) min[Z] = (triangles[i].v1.z);
			
			fscanf(f_pt, "v2 %f %f %f %f %f %f %d\n",
					&(triangles[i].v2.x), &(triangles[i].v2.y), &(triangles[i].v2.z),
					&(triangles[i].normals[2].x), &(triangles[i].normals[2].y), &(triangles[i].normals[2].z),
					&(color_index[2]));
			if(max[X] < (triangles[i].v2.x)) max[X] = (triangles[i].v2.x);
			if(max[Y] < (triangles[i].v2.y)) max[Y] = (triangles[i].v2.y);
			if(max[Z] < (triangles[i].v2.z)) max[Z] = (triangles[i].v2.z);
			if(min[X] > (triangles[i].v2.x)) min[X] = (triangles[i].v2.x);
			if(min[Y] > (triangles[i].v2.y)) min[Y] = (triangles[i].v2.y);
			if(min[Z] > (triangles[i].v2.z)) min[Z] = (triangles[i].v2.z);
			
			fscanf(f_pt, "face normal %f %f %f\n", &(triangles[i].t_normal.x), &(triangles[i].t_normal.y), &(triangles[i].t_normal.z));
				
			triangles[i].color.r = (unsigned char)(int)(255 * diffuse[color_index[0]].r);
			triangles[i].color.g = (unsigned char)(int)(255 * diffuse[color_index[0]].g);
			triangles[i].color.b = (unsigned char)(int)(255 * diffuse[color_index[0]].b);
		}
	fclose(f_pt);
	}
}

void Mesh::print() {
	cout << name << endl;
	cout << n_triangles << endl;
	
	for(int i=0; i<n_triangles; i++) {
		cout << "v0:(" << triangles[i].v0.x << "," << triangles[i].v0.y << "," << triangles[i].v0.z << ")" << endl;
		cout << "v1:(" << triangles[i].v1.x << "," << triangles[i].v1.y << "," << triangles[i].v1.z << ")" << endl;
		cout << "v2:(" << triangles[i].v2.x << "," << triangles[i].v2.y << "," << triangles[i].v2.z << ")" << endl;
		cout << "v0 normal:<" << triangles[i].normals[0].x << "," << triangles[i].normals[0].y << "," << triangles[i].normals[0].z << ">" << endl;
		cout << "v1 normal:<" << triangles[i].normals[1].x << "," << triangles[i].normals[1].y << "," << triangles[i].normals[1].z << ">" << endl;
		cout << "v2 normal:<" << triangles[i].normals[2].x << "," << triangles[i].normals[2].y << "," << triangles[i].normals[2].z << ">" << endl;
		cout << "Face normal:<" << triangles[i].t_normal.x << "," << triangles[i].t_normal.y << "," << triangles[i].t_normal.z << ">" << endl;
		cout << "Triangle color (" << triangles[i].color.r << "," << triangles[i].color.g << "," << triangles[i].color.b << ")" << endl;
	}
}
