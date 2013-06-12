#include "helper.hpp"

#include <memory>
#include <stdexcept>
#include <fstream>
#include <iterator>

using namespace std;

namespace wielgl {

static void glGenericAssert(
	GLuint object,
	GLenum pname,
	void (*getfun)(GLuint, GLenum, GLint*),
	void (*logfun)(GLuint, GLsizei, GLsizei*, GLchar*)
) {
	GLint status;
	getfun(object, pname, &status);
	if(!status) {
		GLint length;
		getfun(object, GL_INFO_LOG_LENGTH, &length);
		unique_ptr<char> s(new char[length]);
		logfun(object, length, nullptr, s.get());
		throw std::runtime_error(s.get());
	}
}

static inline void glProgramAssert(GLuint program, GLenum pname) {
	glGenericAssert(program, pname, glGetProgramiv, glGetProgramInfoLog);
}

static inline void glShaderAssert(GLuint shader, GLenum pname) {
	glGenericAssert(shader, pname, glGetShaderiv, glGetShaderInfoLog);
}

std::string readFile(const std::string& path) {
	std::ifstream in(path.c_str());
	return std::string(
		std::istreambuf_iterator<char>(in),
		std::istreambuf_iterator<char>()
	);
}

GLuint createShader(std::string source, GLenum type) {
	GLuint shader = glCreateShader(type);

	if(!shader)
		throw std::runtime_error("Unable to create shader object.");

	const GLchar* s[] = {source.c_str()};
	const GLint   l[] = {static_cast<GLint>(source.length())};
	glShaderSource(shader, 1, s, l);
	glCompileShader(shader);

	glShaderAssert(shader, GL_COMPILE_STATUS);

	return shader;
}

GLuint createProgram(std::vector<GLuint> shaders) {
	GLuint program = glCreateProgram();

	if(!program)
		throw std::runtime_error("Unable to create program object.");

	for(auto s : shaders)
		glAttachShader(program, s);

	glLinkProgram(program);
	glProgramAssert(program, GL_LINK_STATUS);

	glValidateProgram(program);
	glProgramAssert(program, GL_VALIDATE_STATUS);

	return program;
}

GLint uniformLocation(GLuint program, const std::string& name) {
	GLint location = glGetUniformLocation(program, "transform");
	if(location == -1)
		throw std::runtime_error("\"" + name + "\" uniform var not found.");
	return location;
}

}//namespace wielgl
