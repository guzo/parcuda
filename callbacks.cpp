#include "callbacks.hpp"
#include "particles.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <iostream>
#include <thread>
#include <chrono>

using namespace std;
using namespace glm;

static vec3 rot, mov;

void reshapeFunc(int w, int h) {
	scene.perspective = glm::perspective(60.f, (float)w/(float)h, .1f, 40.f);
	glViewport(0,0,w,h);
	glutPostRedisplay();
}

void displayFunc() {

	glClear(GL_COLOR_BUFFER_BIT);

	glm::mat4 modelview = glm::scale(glm::mat4(1), vec3(1,.5,3));

	glm::mat4 projection = glm::rotate(
		glm::rotate(
			glm::translate(
				scene.perspective,
				vec3(0.f,0.f,-scene.d.z)
			),
			scene.angle.x,
			vec3(1.f,0.f,0.f)
		),
		scene.angle.y,
		vec3(0.f,1.f,0.f)
	);

	glm::mat4 transform = projection*modelview;

	glUniformMatrix4fv(scene.uMVPTransform, 1, GL_FALSE, reinterpret_cast<GLfloat*>(&transform));

	glEnableVertexAttribArray(0);
	glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, scene.vbo);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 2*sizeof(vec3), 0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 2*sizeof(vec3), reinterpret_cast<GLvoid*>(sizeof(vec3)));
		glDrawArrays(GL_POINTS, 0, scene.particle_count);
	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);

	glutSwapBuffers();
}

void keyboardFunc(unsigned char key, int /*x*/, int /*y*/) {
	switch(key) {
		case 'a':
			mov.z =  1;
			break;
		case 'z':
			mov.z = -1;
			break;
		case 'f':
		case 'F':
		case 't':
		case 'T':
			glutFullScreenToggle();
			break;
		case 'q':
		case 'Q':
		case  27: //Esc
			exit(EXIT_SUCCESS);
			break;
		default:
			;//cerr << "unknown key " << static_cast<int>(key) << endl;
	}
}

void keyboardUpFunc(unsigned char key, int /*x*/, int /*y*/) {
	switch(key) {
		case 'a':
		case 'z':
			mov.z = 0;
			break;
		case 'd':
			cerr << scene.angle.x << " " << scene.angle.y << endl;
			break;
	}
}

void specialFunc(int key, int /*x*/, int /*y*/) {
	switch(key) {
		case GLUT_KEY_LEFT:
			rot.y =  1;
			break;
		case GLUT_KEY_RIGHT:
			rot.y = -1;
			break;
		case GLUT_KEY_UP:
			rot.x =  1;
			break;
		case GLUT_KEY_DOWN:
			rot.x = -1;
			break;
	}
}

void specialUpFunc(int key, int /*x*/, int /*y*/) {
	switch(key) {
		case GLUT_KEY_LEFT:
		case GLUT_KEY_RIGHT:
			rot.y = 0;
			break;
		case GLUT_KEY_UP:
		case GLUT_KEY_DOWN:
			rot.x = 0;
			break;
	}
}

void idleFunc() {
	static const auto dt   = std::chrono::milliseconds(30);
	static       auto last = chrono::steady_clock::now();

	const float delta_a = 2;
	const float delta_l = .1;
	scene.angle.x += rot.x*delta_a;
	scene.angle.y += rot.y*delta_a;
	scene.d.z += mov.z*delta_l;

	particles_step(scene.particle_count);

	glutPostRedisplay();

	if(chrono::steady_clock::now() > last+dt)
		cout << "missed" << endl;
	std::this_thread::sleep_until(last + dt);
	last = chrono::steady_clock::now();
}
