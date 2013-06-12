#ifndef _SCENE_HPP_
#define _SCENE_HPP_

#include <glm/glm.hpp>

#include <GL/glew.h>

struct Scene {
	GLuint particle_count;
	GLuint vbo;
	GLint uMVPTransform;
	glm::vec3 angle, d;
	glm::mat4 perspective;

	struct {
		GLuint color, ambient, diffuse, specular, normal;
	} light;
};

#endif//_SCENE_HPP_
