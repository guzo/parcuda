#include "scene.hpp"
#include "callbacks.hpp"
#include "helper.hpp"
#include "particles.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <GL/glew.h>
#include <GL/freeglut.h>

#include <cuda.h>
#include <cuda_runtime_api.h>
#include <cuda_gl_interop.h>

#include <type_traits>
#include <string>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <vector>

#include <cstring>
#include <cassert>
#include <cmath>

using namespace glm;
using namespace std;
using namespace wielgl;

static_assert(sizeof(glm::vec3) == sizeof(glm::vec3::value_type)*3,   "glm::vec3 not POD enough.");
static_assert(sizeof(glm::mat4) == sizeof(glm::vec4::value_type)*4*4, "glm::mat4 not POD enough.");

Scene scene;
cudaGraphicsResource_t cudavbo;

static void initBuffers() {
	glGenBuffers(1, &scene.vbo);
	glBindBuffer(GL_ARRAY_BUFFER, scene.vbo);
	glBufferData(GL_ARRAY_BUFFER, scene.particle_count*2*sizeof(vec3), 0, GL_DYNAMIC_DRAW);
	WRAP_CUDA(cudaGraphicsGLRegisterBuffer(&cudavbo, scene.vbo, cudaGraphicsRegisterFlagsNone));

	particles_init(scene.particle_count);
}

static void oglInit() {
	glutInitDisplayMode(GLUT_DOUBLE|GLUT_RGBA);
	glutInitWindowSize(640, 480);
	glutCreateWindow("waterfall");

	glutReshapeFunc(reshapeFunc);
	glutDisplayFunc(displayFunc);
	glutKeyboardFunc(keyboardFunc);
	glutKeyboardUpFunc(keyboardUpFunc);
	glutSpecialFunc(specialFunc);
	glutSpecialUpFunc(specialUpFunc);
	glutIdleFunc(idleFunc);

	GLenum glewStatus = glewInit();
	if(glewStatus != GLEW_OK) {
		cerr << "GLEW says \"" <<  glewGetErrorString(glewStatus) << '\"' << endl;
		exit(EXIT_FAILURE);
	}

	WRAP_CUDA(cudaGLSetGLDevice(0));
}

static void sceneInit() {
	glClearColor(0, 0, 0, 0);
	glEnable(GL_PROGRAM_POINT_SIZE);

	scene.angle.x = 18;
	scene.angle.y = 46;
	scene.d.z     = 5;
	scene.particle_count = PX*PY*PZ;
	initBuffers();

	GLuint program = createProgram(std::vector<GLuint>{
		createShader(readFile("vs.glsl"), GL_VERTEX_SHADER  ),
		createShader(readFile("fs.glsl"), GL_FRAGMENT_SHADER),
	});

	glUseProgram(program);

	scene.uMVPTransform  = glGetUniformLocation(program, "transform");
	scene.light.color    = glGetUniformLocation(program, "light.color");
	scene.light.ambient  = glGetUniformLocation(program, "light.ambient");
	scene.light.diffuse  = glGetUniformLocation(program, "light.diffuse");
	scene.light.specular = glGetUniformLocation(program, "light.specular");
	scene.light.normal   = glGetUniformLocation(program, "light.normal");

	glUniform3f(scene.light.color,    1., 1., 1.);
	glUniform1f(scene.light.ambient,   .5);
	glUniform1f(scene.light.diffuse,   .5);
	glUniform1f(scene.light.specular,  .1);
	glUniform3f(scene.light.normal,   -1., -1., -1.);
}

int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	oglInit();
	sceneInit();
	glutMainLoop();

	return 0;
}
