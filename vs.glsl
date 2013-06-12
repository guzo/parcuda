#version 330

layout (location = 0) in vec3 pos;
layout (location = 1) in vec3 norm;

out vec3 vcolor;
out vec3 vnormal;

uniform mat4 transform;

void main() {
	gl_Position = transform*vec4(pos, 1);
	gl_PointSize = 2.f;
	vcolor = clamp((pos+1),0,1);
	vnormal = norm;
}
