#include <iostream>
#include "vector3f.h"
#include "mesh.h"
#include "camera.h"
#include "close2gl.h"
#include <GL/glut.h>
#include <GL/glui.h>

#define WIDTH			640
#define HEIGHT			480
#define WINS 			2
#define OPENGL_WIN 		0
#define CLOSE2GL_WIN 	1
#define INC				6.5

int wins[WINS];
int w = WIDTH;
int h = HEIGHT;

close2gl Close2GL;

Camera cam;
Mesh m1;

vector3f translate(0.0, 0.0, 0.0);
vector3f rotation;
Color color = {1.0, 0, 0};
int lookat = 0;
int vertex_orientation = 1;
int polygon = 0;
int wireframe = 1;
int point = 0;
int bfculling = 0;
int lighting = 0;
char file[50];

//prototypes
void setGl();

void reset() {
	translate.x = translate.y = translate.z = 0;
	rotation.x  = rotation.y  = rotation.z  = 0;
	GLUI_Master.sync_live_all();
}

void load() {
	m1.name = file;
	m1.readFromFile();
	Close2GL = close2gl(w, h, 0, 0, &cam, &m1);
}

void display() {
    
    setGl();
    
    cam.set(m1.max, m1.min, w, h, translate, rotation, lookat);
    	
    glColor3fv(&(color.r));
    
	if(wireframe) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		polygon = 0; point = 0;
	}
	else if(point) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
		wireframe = 0; polygon = 0;
	}
	else if(polygon) {
		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		wireframe = 0; point = 0;
	}

	for(unsigned int i=0; i<m1.n_triangles; i++) {
		glBegin(GL_TRIANGLES);
			glNormal3fv(&(m1.triangles[i].normals[0].x));
			glVertex3fv(&(m1.triangles[i].v0.x));
			glNormal3fv(&(m1.triangles[i].normals[1].x));
			glVertex3fv(&(m1.triangles[i].v1.x));
			glNormal3fv(&(m1.triangles[i].normals[2].x));
			glVertex3fv(&(m1.triangles[i].v2.x));
		glEnd();
	}
    glutSwapBuffers();
    GLUI_Master.sync_live_all();
}

void displayC2GL() {
	//clear buffers
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glClearDepth(1.0f);
	
	if(Close2GL.mesh != NULL) {
		Close2GL.mainLoop();

		glColor3fv(&(color.r));

		if(wireframe) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
			polygon = 0; point = 0;
		}
		else if(point) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
			wireframe = 0; polygon = 0;
		}
		else if(polygon) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
			wireframe = 0; point = 0;
		}
		for(unsigned int i=0; i<Close2GL.n_clipped_triangles; i++) {
//			cout<< i <<" garrafa cerveja no muro" << endl;
			glBegin(GL_TRIANGLES);
				glVertex2fv(&(Close2GL.clipped_triangles[i].v0.vec.x));
				glVertex2fv(&(Close2GL.clipped_triangles[i].v1.vec.x));
				glVertex2fv(&(Close2GL.clipped_triangles[i].v2.vec.x));
			glEnd();
		}
		cout << "Drawn " << Close2GL.n_clipped_triangles << " triangles" << endl;
	}	
    glutSwapBuffers();
	Close2GL.SCStriangles.clear();
	Close2GL.clipped_triangles.clear();
}

void reshape(int w, int h) {
	glViewport(0, 0, (GLsizei) w, (GLsizei) h);
	glMatrixMode(GL_PROJECTION);
//	glLoadIdentity();
//	glOrtho(-5.0, 5.0, -5.0, 5.0, -20.0, 20.0);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
}

void idle() {
	glutSetWindow(wins[OPENGL_WIN]);
	glutPostRedisplay();
	glutSetWindow(wins[CLOSE2GL_WIN]);
	glutPostRedisplay();
}

//TODO: fovy and fovx(calculate fovy from fovx)
void keyboard(unsigned char key, int x, int y) {
	switch(key) {
		//u-axis rotation
		case 'z':
			rotation.x -= INC+5;
			break;
		case 'x':
			rotation.x += INC+5;
			break;
		//v-axis rotation
		case 'c':
			rotation.y -= INC+5;
			break;
		case 'v':
			rotation.y += INC+5;
			break;
		//z-axis rotation
		case 'b':
			rotation.z -= INC+5;
			break;
		case 'n':
			rotation.z += INC+5;
			break;
		//z-near translation
		case 'a':
			cam.znear -= INC;
			break;
		case 's':
			cam.znear += INC;
			break;
		//z-far translation
		case 'd':
			cam.zfar -= INC;
			break;
		case 'f':
			cam.zfar += INC;
			break;			
			
		//other commands
		case 'h':
			vertex_orientation = !vertex_orientation;
			break;
		case 'j':
			bfculling = !bfculling;
			break;
		case 'k':
			lighting = !lighting;
			break;
		case 'l':
			lookat = !lookat;
			break;
		case 'r':
			reset();
			break;
		case 'w':
			wireframe = 1; polygon = 0; point = 0;
			break;
		case 'p':
			wireframe = 0; polygon = 0; point = 1;
			break;
		case 't':
			wireframe = 0; polygon = 1; point = 0;
			break;	
	}
	GLUI_Master.sync_live_all();
}

void specialFunc(int key, int x, int y) {
	switch(key) {
		//u-axis translation
		case GLUT_KEY_LEFT:
			translate.x -= INC;
			break;
		case GLUT_KEY_RIGHT:
			translate.x += INC;
			break;
		//v-axis translation
		case GLUT_KEY_DOWN:
			translate.y -= INC;
			break;
		case GLUT_KEY_UP:
			translate.y += INC;
			break;
	}
	GLUI_Master.sync_live_all();
}

void mouse(int button, int state, int x, int y) {
	//z-axis translation
	if(button == 3)	//scroll up
		translate.z -= INC;
	if(button == 4)	//scroll down
		translate.z += INC;
    
	GLUI_Master.sync_live_all();
}

void setGlui() {
	GLUI *glui = GLUI_Master.create_glui ("OPTIONS", 0, w, 0);
	
	GLUI_Panel *obj_panel = glui->add_panel ("Camera translation");
	GLUI_Spinner *segment_spinner  = glui->add_spinner_to_panel (obj_panel, "x", GLUI_SPINNER_FLOAT, &(translate.x));
	GLUI_Spinner *segment_spinner2 = glui->add_spinner_to_panel (obj_panel, "y", GLUI_SPINNER_FLOAT, &(translate.y));
	GLUI_Spinner *segment_spinner3 = glui->add_spinner_to_panel (obj_panel, "z", GLUI_SPINNER_FLOAT, &(translate.z));	
	glui->add_checkbox_to_panel(obj_panel, "Look at object", &lookat, NULL);	
	
	GLUI_Panel *obj_panel2 = glui->add_panel ("Camera rotation");
	GLUI_Spinner *segment_spinner4 = glui->add_spinner_to_panel (obj_panel2, "x", GLUI_SPINNER_FLOAT, &(rotation.x));
	segment_spinner4->set_float_limits(0, 360, GLUI_LIMIT_WRAP);
	GLUI_Spinner *segment_spinner5 = glui->add_spinner_to_panel (obj_panel2, "y", GLUI_SPINNER_FLOAT, &(rotation.y));
	segment_spinner5->set_float_limits(0, 360, GLUI_LIMIT_WRAP);
	GLUI_Spinner *segment_spinner6 = glui->add_spinner_to_panel (obj_panel2, "z", GLUI_SPINNER_FLOAT, &(rotation.z));
	segment_spinner6->set_float_limits(0, 360, GLUI_LIMIT_WRAP);
	
	GLUI_Panel *obj_panel3 = glui->add_panel ("Rendering");
	glui->add_checkbox_to_panel(obj_panel3, "CW vertex order?", &vertex_orientation, NULL);	
	GLUI_Spinner *segment_spinner7 = glui->add_spinner_to_panel (obj_panel3, "z near:", GLUI_SPINNER_FLOAT, &(cam.znear));
	segment_spinner7->set_float_limits(0, 9999, GLUI_LIMIT_CLAMP);
	GLUI_Spinner *segment_spinner8 = glui->add_spinner_to_panel (obj_panel3, "z far:", GLUI_SPINNER_FLOAT, &(cam.zfar));
	segment_spinner8->set_float_limits(0, 9999, GLUI_LIMIT_CLAMP);
	GLUI_Spinner *segment_spinner12 = glui->add_spinner_to_panel (obj_panel3, "fov y:", GLUI_SPINNER_FLOAT, &(cam.fovy));
	segment_spinner12->set_float_limits(0, 9999, GLUI_LIMIT_CLAMP);
	glui->add_checkbox_to_panel(obj_panel3, "Points", &point, NULL);	
	glui->add_checkbox_to_panel(obj_panel3, "Wireframe", &wireframe, NULL);
	glui->add_checkbox_to_panel(obj_panel3, "Polygons", &polygon, NULL);
	glui->add_checkbox_to_panel(obj_panel3, "Backface culling", &bfculling, NULL);
	glui->add_checkbox_to_panel(obj_panel3, "Lighting", &lighting, NULL);

	GLUI_Panel *obj_panel4 = glui->add_panel ("Color");
	GLUI_Spinner *segment_spinner9 = glui->add_spinner_to_panel (obj_panel4, "R:", GLUI_SPINNER_FLOAT, &(color.r));
	segment_spinner9->set_float_limits(0.0, 1.0, GLUI_LIMIT_CLAMP);
	GLUI_Spinner *segment_spinner10 = glui->add_spinner_to_panel (obj_panel4, "G:", GLUI_SPINNER_FLOAT, &(color.g));
	segment_spinner10->set_float_limits(0.0, 1.0, GLUI_LIMIT_CLAMP);
	GLUI_Spinner *segment_spinner11 = glui->add_spinner_to_panel (obj_panel4, "B:", GLUI_SPINNER_FLOAT, &(color.b));
	segment_spinner11->set_float_limits(0, 1, GLUI_LIMIT_CLAMP);
	
	GLUI_Panel *obj_panel5 = glui->add_panel ("");
	GLUI_Button *button = glui->add_button_to_panel (obj_panel5, "Reset Camera", NULL, GLUI_Update_CB(reset));
	GLUI_EditText *edittext = glui->add_edittext_to_panel (obj_panel5, "File name:", GLUI_EDITTEXT_TEXT, file, NULL);
	GLUI_Button *button2 = glui->add_button_to_panel (obj_panel5, "Load Model", NULL, GLUI_Update_CB(load));
	
	glui->set_main_gfx_window(wins[OPENGL_WIN]);
	glutSetWindow(wins[OPENGL_WIN]);
	GLUI_Master.set_glutIdleFunc(idle);
}

void setGl() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glClearDepth(1.0f);
	glDepthFunc(GL_LEQUAL);

	GLfloat light_position[] = {1.0, 1.0, -1.0, 0.0};
	GLfloat light_ambient[] = {0.0, 0.0, 0.0, 1.0};
	GLfloat light_diffuse[] = {1.0, 1.0, 1.0, 1.0};
	GLfloat light_specular[] = {1.0, 1.0, 1.0, 1.0};

	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);

	glEnable(GL_COLOR_MATERIAL);
	
	if(lighting) {
		glEnable(GL_LIGHTING);
		glEnable(GL_LIGHT0);
	}
	else {
		glDisable(GL_LIGHTING);
		glDisable(GL_LIGHT0);
	}
	
	if(vertex_orientation)
	    glFrontFace(GL_CW);
	else
		glFrontFace(GL_CCW);
		
	if(bfculling) {
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}
	else
		glDisable(GL_CULL_FACE);
    
	
   	glEnable(GL_DEPTH_TEST);

	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
	glShadeModel(GL_SMOOTH);
}

void setGlut() {
	//intialize GLUT in the default way
	//aux and aux2 are passed to glutInit to make it initialize glut in default way
	char *aux = "";
	int aux2 = 1;
	glutInit(&aux2, &aux);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); 
	glutInitWindowPosition(0, 0); 
	glutInitWindowSize(w,h); 
	wins[OPENGL_WIN] = glutCreateWindow("OpenGL"); 
	glutDisplayFunc(display); 
	glutReshapeFunc(reshape); 
	glutIdleFunc(idle);
	glutSpecialFunc(specialFunc);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);

	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH); 
	glutInitWindowPosition(0+w, 0); 
	glutInitWindowSize(w,h); 
	wins[CLOSE2GL_WIN] = glutCreateWindow("Close2GL"); 
	glutDisplayFunc(displayC2GL); 
	glutReshapeFunc(reshape); 
	glutIdleFunc(idle);
	glutSpecialFunc(specialFunc);
	glutKeyboardFunc(keyboard);
	glutMouseFunc(mouse);
}

int main(int argc, char** argv) {	
	
	if(argc == 2) {
		m1.name = argv[1];
		m1.readFromFile();
		Close2GL = close2gl(w, h, 0, 0, &cam, &m1);
	}	
	
	setGlut();
	setGlui();
	
	glutMainLoop();
	
	return 0;
}
