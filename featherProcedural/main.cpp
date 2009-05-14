#include <iostream>
#include "FXMLTriangleMap.h"
#include "../shared/gPerspCamera.h"
#include "../shared/gBase.h"

static gPerspCamera* m_pCamera;
static int m_pos_x, m_pos_y, m_button;
static int m_wnd_width, m_wnd_height;
static void keyboard(unsigned char key, int x, int y);
static void display();

static FXMLTriangleMap* mesh;
static DFeather* feather;

static float m_wind_x, m_wind_y;
static void display() 
{
	m_pCamera->setViewPort(41.0, m_wnd_width, m_wnd_height);
	glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   m_pCamera->update();
    glColor3f(.2,.2,.1);
	glFrontFace(GL_CCW);
	mesh->draw();
	glColor3f(.1,.2,.6);
	glFrontFace(GL_CW);
	glEnable(GL_LIGHTING);
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	feather->drawQuad();
	glDisable(GL_LIGHTING);

	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
   gBase::drawCoordinate();
  
   glFlush();
    glutSwapBuffers();
}

static void keyboard(unsigned char key, int x, int y)
{
    if (key == 'q' || key == 'Q' || key == 27 /* escape */) 
	{
				exit(0);
	}
	else if(key=='f')
	{
		XYZ cen;
		float sz;
		mesh->getCenterAndSize(cen, sz);
		m_pCamera->viewAll(cen, sz);
		glutPostRedisplay();
	}
}

void processMouse(int button, int state, int x, int y) 
{

	// if both a mouse button, and the ALT key, are pressed  then
	if (state == GLUT_DOWN) 
	{
		m_pos_x = x; m_pos_y = y;
	}
	else 
	{
		glutPostRedisplay();
	}
		
		if (button == GLUT_LEFT_BUTTON) {
			m_button = 0;
		}
		else if (button == GLUT_MIDDLE_BUTTON) {
			m_button = 2;
		}
		else {
			m_button = 1;
		}
	
}

void processMouseActiveMotion(int x, int y) 
{
		int dx = x - m_pos_x;
		int dy = y - m_pos_y;
		
		m_pos_x = x; m_pos_y = y;
		
		if(m_button == 0) m_pCamera->tumble( (float)dx, (float)dy );
		else if (m_button == 1) m_pCamera->dolly( (float)dx+dy );
		else m_pCamera->track( (float)dx, (float)dy );
		
		glutPostRedisplay();
	
}

void changeSize(int w, int h)
{
	m_wnd_width = w;
	m_wnd_height = h;
	
	m_pCamera->setViewPort(m_wnd_width, m_wnd_height);

}

void specialKey( int key, int x, int y)
{

	if(key==GLUT_KEY_LEFT)
	{
		m_wind_y++;
		glutPostRedisplay();
		cout<<" y "<<m_wind_y;
		return;
	}
	
	if(key==GLUT_KEY_RIGHT)
	{
		m_wind_y--;
		glutPostRedisplay();
		cout<<" y "<<m_wind_y;
		return;
	}
	
	if(key==GLUT_KEY_UP)
	{
		m_wind_x++;
		glutPostRedisplay();
		cout<<" x "<<m_wind_x;
		return;
	}
	
	if(key==GLUT_KEY_DOWN)
	{
		m_wind_x--;
		glutPostRedisplay();
		cout<<" x "<<m_wind_x;
		return;
	}
}


void initGL()
{
	//   	glPolygonMode(GL_FRONT_AND_BACK,GL_LINE);
glEnable(GL_DEPTH_TEST);
glDepthFunc(GL_LESS);
glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	
//glEnable(GL_POINT_SMOOTH);
glPointSize(2.0f);
    // The usual OpenGL stuff to clear the screen and set up viewing.
    glClearColor(.0, .0, .0, .0);
	glClearDepth(1.0f);
	//glEnable(GL_CULL_FACE);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
	
	GLfloat lightAmbient[] = {.5f, .5f, .5f, 1.0f};
	GLfloat lightDiffuse[] = {1.0f, 1.0f, 1.0f, 1.0f};
	GLfloat lightPosition[] = {400.0f, 400.0f, 600.0f, 1.0f};
	glLightfv(GL_LIGHT1, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT1, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT1, GL_POSITION, lightPosition);
	glEnable(GL_LIGHT1);
	
}

int main (int argc, char * argv[]) {

	glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(640, 480);
    glutCreateWindow("Feather Viewer");
	
	glutMouseFunc(processMouse);
	glutMotionFunc(processMouseActiveMotion);
    glutKeyboardFunc(keyboard);
	glutSpecialFunc(specialKey);
    glutDisplayFunc(display);
	glutReshapeFunc(changeSize);
	
	XYZ origin(25,25,25);
	m_pCamera = new gPerspCamera( 41.0f, 0.1f, 500.0f, origin );
	
	initGL();

	//cout<<"Loading "<<argv[1]<<endl;
	
    // insert code here...
    std::cout << "Enter number of feather: ";
	int x;
	cin>>x;
	
	std::cout << "Enter width/height ratio: ";
	float r;
	cin>>r;
	//mesh = new FXMLTriangleMap("/Users/jollyroger/zhang/featherProcedural/featherViz1.11.xhb");
	if(argc < 1) mesh = new FXMLTriangleMap("/Users/jollyroger/zhang/featherProcedural/featherViz1.12.xhb");
	else mesh = new FXMLTriangleMap(argv[1]);
	
	//cout<<"n tri "<<mesh->getNumFace();
	//cout<<"n face vert "<<mesh->getNumFaceVertex();
	//cout<<"area "<<mesh->getArea();
	
	feather = new DFeather();
	feather->setWidth(r);
	mesh->dice(x, feather);
	m_wind_y = 10;
	m_wind_x = 0;
	
	glutMainLoop();
	
    return 0;
}
