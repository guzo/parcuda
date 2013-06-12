#ifndef _HELPER_HPP_
#define _HELPER_HPP_

#include <GL/glew.h>
#include <string>
#include <vector>

namespace wielgl {

std::string readFile(const std::string& path);

GLuint createShader(std::string source, GLenum type);
GLuint createProgram(std::vector<GLuint> shaders);

GLint uniformLocation(GLuint program, const std::string& name);

}//namespace wielgl

#endif//_HELPER_HPP_
