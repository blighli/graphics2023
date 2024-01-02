#include <cmath>
#include <cstdlib>
#ifdef _WIN32
#include <Windows.h>
#endif
#include <gl\gl.h>
#include <glut.h>
#include <GL\glu.h>

#include "tga.h"
#include "solarsystem.h"
#include "camera.h"
#include "globals.h"
// the screen size
int screenWidth, screenHeight;

// The TGA texture containing the help dialogue and starfield and moon texture
TGA* help, *stars, * moon;
// toggles if the help dialogue is enabled
bool helpDialogue = true;
// toggles if orbits are drawn
bool showOrbits = true;
// the real size of planets is small
bool showRealsize = false;
// holds the index of the last planet that was selected with the 1 to 9 number keys
int planetSelected = 1;

// The main instance of the solar system
SolarSystem solarSystem;

// The instance of the camera
Camera camera;

// These control the simulation of time
double time;
double timeSpeed;

// holds the state of the controls for the camera - when true, the key for that control is being pressed
struct ControlStates
{
	bool forward, backward, left, right, yawLeft, yawRight, pitchUp,
		pitchDown, rollLeft, rollRight;
} controls;


// timer function called every 10ms or more
void timer(int)
{
	glutPostRedisplay(); // post for display func
	glutTimerFunc(10, timer, 0); // limit frame drawing to 100fps
}

// creates a random number up to the max specified
float random(float max)
{
	return (float)(std::rand() % 1000) * max * 0.001;
}

// adds a moon to the selected planet
void addMoon()
{
	// make a moon using random values
	solarSystem.addMoon(planetSelected, 
		(500 + random(1500)) * solarSystem.getRadiusOfPlanet(planetSelected),
		10 + random(100), 0.5 + random(20),
		solarSystem.getRadiusOfPlanet(planetSelected) * (0.05f + random(0.2f)), moon->getTextureHandle());
}


void init(void)
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glShadeModel(GL_SMOOTH);
	glEnable(GL_TEXTURE_2D);
	
	// set up lighting
	glEnable(GL_LIGHTING);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	GLfloat matSpecular[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat matAmbience[] = { 0.3, 0.3, 0.3, 1.0 };
	GLfloat matShininess[] = { 20.0 };
	glClearColor (0.0, 0.0, 0.0, 0.0);
	glShadeModel (GL_SMOOTH);

	glMaterialfv(GL_FRONT, GL_SPECULAR, matSpecular);
	glMaterialfv(GL_FRONT, GL_SHININESS, matShininess);
	glMaterialfv(GL_FRONT, GL_AMBIENT, matAmbience);

	GLfloat lightAmbient[] = { 0.3, 0.3, 0.3, 1.0 };
	GLfloat lightDiffuse[] = { 1.0, 1.0, 1.0, 1.0 };
	GLfloat lightSpecular[] = { 1.0, 1.0, 1.0, 1.0 };

	glLightfv(GL_LIGHT0, GL_AMBIENT, lightAmbient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, lightDiffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, lightSpecular);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glDisable(GL_LIGHTING);



	char image_1[] = "images/help-blue.tga";
	char image_2[] = "images/stars2.tga";
	char image_3[] = "images/moon.tga";

	char image_4[] = "images/sun.tga";
	char image_5[] = "images/mercury.tga";
	char image_6[] = "images/venus.tga";
	char image_7[] = "images/earth.tga";
	char image_8[] = "images/mars.tga";
	char image_9[] = "images/jupiter.tga";
	char image_10[] = "images/saturn.tga";

	char image_11[] = "images/uranus.tga";
	char image_12[] = "images/neptune.tga";
	char image_13[] = "images/pluto.tga";
	// Load all image data
	help = new TGA(image_1);
	stars = new TGA(image_2);
	moon = new TGA(image_3);

	TGA* sun = new TGA(image_4);
	TGA* mercury = new TGA(image_5);
	TGA* venus = new TGA(image_6);
	TGA* earth = new TGA(image_7);
	TGA* mars = new TGA(image_8);
	TGA* jupiter = new TGA(image_9);
	TGA* saturn = new TGA(image_10);
	TGA* uranus = new TGA(image_11);
	TGA* neptune = new TGA(image_12);
	TGA* pluto = new TGA(image_13);

	// Add all the planets with accurate data. Distance measured in km, time measured in earth days.
	solarSystem.addPlanet(0, 1, 500, 695500, sun->getTextureHandle()); // sun
	solarSystem.addPlanet(57910000, 88, 58.6, 2440, mercury->getTextureHandle()); // mercury
	solarSystem.addPlanet(108200000, 224.65, 243, 6052, venus->getTextureHandle()); // venus
	solarSystem.addPlanet(149600000, 365, 1, 6371, earth->getTextureHandle()); // earth
	solarSystem.addPlanet(227939100, 686, 1.03f, 3389, mars->getTextureHandle()); // mars
	solarSystem.addPlanet(778500000, 4332, 0.4139, 69911, jupiter->getTextureHandle()); // jupiter
	solarSystem.addPlanet(1433000000, 10759, 0.44375, 58232, saturn->getTextureHandle()); // saturn
	solarSystem.addPlanet(2877000000, 30685, 0.718056, 25362, uranus->getTextureHandle()); // uranus
	solarSystem.addPlanet(4503000000, 60188, 0.6713, 24622, neptune->getTextureHandle()); // neptune
	solarSystem.addPlanet(5906380000, 90616, 6.39, 1137, pluto->getTextureHandle()); // pluto

	solarSystem.addMoon(3, 7000000, 27.3, 27.3, 1738, moon->getTextureHandle()); // test moon for the earth

	// set up time
	time = 2.552f;
	timeSpeed = 0.1f;

	// reset controls
	controls.forward = false;
	controls.backward = false;
	controls.left = false;
	controls.right = false;
	controls.rollRight = false;
	controls.rollLeft = false;
	controls.pitchDown = false;
	controls.pitchUp = false;
	controls.yawLeft = false;
	controls.yawRight = false;

	timer(0);
}

void drawCube(void);

void mouseClick(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		GLint viewport[4];
		GLdouble modelview[16];
		GLdouble projection[16];
		GLfloat winX, winY, winZ;
		GLdouble posX, posY, posZ;

		glGetIntegerv(GL_VIEWPORT, viewport);
		glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
		glGetDoublev(GL_PROJECTION_MATRIX, projection);

		winX = (float)x;
		winY = (float)viewport[3] - (float)y;
		glReadPixels(x, int(winY), 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);

		gluUnProject(winX, winY, winZ, modelview, projection, viewport, &posX, &posY, &posZ);

		for (int i = 0; i < 10; ++i) {
			float planetPos[3];
			solarSystem.getPlanetPosition(i, planetPos);

			// Apply the same transformations to the planet's position
			// as during rendering to get the correct world coordinates.
			glPushMatrix();
			glTranslatef(planetPos[0] * distanceScale, planetPos[1] * distanceScale, planetPos[2] * distanceScale);
			glGetDoublev(GL_MODELVIEW_MATRIX, modelview); 
			glPopMatrix();


			float distance = sqrt(pow(posX - planetPos[0], 2) + pow(posY - planetPos[1], 2) + pow(posZ - planetPos[2], 2));

			if (distance < solarSystem.getRadiusOfPlanet(i) * planetSizeScale) {
				printf("Clicked on %d\n", i);
				break;
			}
		}
	}
}


void display(void)
{
	// update the logic and simulation
	time += timeSpeed;
	solarSystem.calculatePositions(time);

	if (controls.forward) camera.forward();		if (controls.backward) camera.backward();
	if (controls.left) camera.left();			if (controls.right) camera.right();
	if (controls.yawLeft) camera.yawLeft();		if (controls.yawRight) camera.yawRight();
	if (controls.rollLeft) camera.rollLeft();	if (controls.rollRight) camera.rollRight();
	if (controls.pitchUp) camera.pitchUp();		if (controls.pitchDown) camera.pitchDown();

	// clear the buffers
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3f(1.0, 1.0, 1.0);

	
	// set up the perspective matrix for rendering the 3d world
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(70.0f, (float)screenWidth / (float)screenHeight, 0.001f, 500.0f);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	

	// perform the camera orientation transform
	camera.transformOrientation();

	// draw the skybox
	glBindTexture(GL_TEXTURE_2D, stars->getTextureHandle());
	drawCube();

	// perform the camera translation transform
	camera.transformTranslation();

	
	
	GLfloat lightPosition[] = { 0.0, 0.0, 0.0, 1.0 };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);
	
	// render the solar system
	glEnable(GL_DEPTH_TEST);
	glEnable(GL_LIGHTING);

	solarSystem.render();
	glDisable(GL_LIGHTING);

	// possibly render orbits
	if (showOrbits)
		solarSystem.renderOrbits();
	
	glDisable(GL_DEPTH_TEST);

	// set up ortho matrix for showing the UI (help dialogue)
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluOrtho2D(0.0, (GLdouble) screenWidth, (GLdouble) screenHeight, 0.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	
	// draw the help dialogue
	if (helpDialogue)
	{
		glBindTexture(GL_TEXTURE_2D, help->getTextureHandle());
		glBegin(GL_QUADS);
			glTexCoord2f(0.0f, 0.0f);	glVertex2f(0.0f, 0.0f);
			glTexCoord2f(1.0f, 0.0f);	glVertex2f(512.0f, 0.0f);
			glTexCoord2f(1.0f, 1.0f);	glVertex2f(512.0f, 512.0f);
			glTexCoord2f(0.0f, 1.0f);	glVertex2f(0.0f, 512.0f);
		glEnd();
	}

	glFlush();
	glutSwapBuffers();
}
void keyDown(unsigned char key, int x, int y)
{
	// check for numerical keys
	if (key > '0' && key <= '9')
	{
		// point at the specified planet
		float vec[3];
		solarSystem.getPlanetPosition(key - '0', vec);
		camera.pointAt(vec);

		// select that planet
		planetSelected = key - '0';
	}
	switch (key)
	{
	case '-':
		timeSpeed /= 2.0f; // half the rate of time passing
		break;
	case '=':
		timeSpeed *= 2.0f; // double the rate of time passing
		break;
	case 'h':
		helpDialogue = !helpDialogue; // toggle the dialogue
		break;
	case '[':
		planetSizeScale /= 1.2; // make planet scale smaller
		break;
	case ']':
		planetSizeScale *= 1.2; // make planet scale bigger
		break;
	case 'o':
		showOrbits = !showOrbits; // toggle show orbits
		break;
	case 'm':
		addMoon(); // add a moon to the selected planet
		break;
	case 'r':
		if (showRealsize == false) {
			planetSizeScale = distanceScale;
			showRealsize = true;
		}
		else {
			planetSizeScale = 0.000005f;
			showRealsize = false;
		}
		break;
	case ',':
		camera.slowDown(); // slow down camera
		break;
	case '.':
		camera.speedUp(); // speed up camera
		break;
		// these are all camera controls
	case 'w':
		controls.forward = true;
		break;
	case 's':
		controls.backward = true;
		break;
	case 'a':
		controls.left = true;
		break;
	case 'd':
		controls.right = true;
		break;
	case 'l':
		controls.rollRight = true;
		break;
	case 'j':
		controls.rollLeft = true;
		break;
	case 'i':
		controls.pitchDown = true;
		break;
	case 'k':
		controls.pitchUp = true;
		break;
	case 'q':
		controls.yawLeft = true;
		break;
	case 'e':
		controls.yawRight = true;
		break;
	}

}

void keyUp(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 'w':
		controls.forward = false;
		break;
	case 's':
		controls.backward = false;
		break;
	case 'a':
		controls.left = false;
		break;
	case 'd':
		controls.right = false;
		break;
	case 'l':
		controls.rollRight = false;
		break;
	case 'j':
		controls.rollLeft = false;
		break;
	case 'i':
		controls.pitchDown = false;
		break;
	case 'k':
		controls.pitchUp = false;
		break;
	case 'q':
		controls.yawLeft = false;
		break;
	case 'e':
		controls.yawRight = false;
		break;
	}
}

void reshape(int w, int h)
{
	screenWidth = w;
	screenHeight = h;
	glViewport (0, 0, (GLsizei) w, (GLsizei) h);
}


int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGB|GLUT_DEPTH);
	glutInitWindowSize(1200, 700);
	glutInitWindowPosition(100, 100);
	glutCreateWindow("Solar System");
	init();
	glutMouseFunc(mouseClick);
	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyDown);
	glutKeyboardUpFunc(keyUp);

	glutMainLoop();
	return 0;
}

void drawCube(void)
{
	glBegin(GL_QUADS);
	// new face
	glTexCoord2f(0.0f, 0.0f);	glVertex3f(-1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f);	glVertex3f(1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f);	glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f);	glVertex3f(-1.0f, 1.0f, 1.0f);
	// new face
	glTexCoord2f(0.0f, 0.0f);	glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(1.0f, 0.0f);	glVertex3f(1.0f, 1.0f, -1.0f);
	glTexCoord2f(1.0f, 1.0f);	glVertex3f(1.0f, -1.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f);	glVertex3f(1.0f, -1.0f, 1.0f);
	// new face
	glTexCoord2f(0.0f, 0.0f);	glVertex3f(1.0f, 1.0f, -1.0f);
	glTexCoord2f(1.0f, 0.0f);	glVertex3f(-1.0f, 1.0f, -1.0f);
	glTexCoord2f(1.0f, 1.0f);	glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord2f(0.0f, 1.0f);	glVertex3f(1.0f, -1.0f, -1.0f);
	// new face
	glTexCoord2f(0.0f, 0.0f);	glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord2f(1.0f, 0.0f);	glVertex3f(-1.0f, -1.0f, 1.0f);
	glTexCoord2f(1.0f, 1.0f);	glVertex3f(-1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f);	glVertex3f(-1.0f, 1.0f, -1.0f);
	// new face
	glTexCoord2f(0.0f, 0.0f);	glVertex3f(-1.0f, 1.0f, -1.0f);
	glTexCoord2f(1.0f, 0.0f);	glVertex3f(1.0f, 1.0f, -1.0f);
	glTexCoord2f(1.0f, 1.0f);	glVertex3f(1.0f, 1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f);	glVertex3f(-1.0f, 1.0f, 1.0f);
	// new face
	glTexCoord2f(0.0f, 0.0f);	glVertex3f(-1.0f, -1.0f, -1.0f);
	glTexCoord2f(1.0f, 0.0f);	glVertex3f(1.0f, -1.0f, -1.0f);
	glTexCoord2f(1.0f, 1.0f);	glVertex3f(1.0f, -1.0f, 1.0f);
	glTexCoord2f(0.0f, 1.0f);	glVertex3f(-1.0f, -1.0f, 1.0f);

	glEnd();
}
