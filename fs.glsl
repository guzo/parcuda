#version 330

in  vec3 vcolor;
in  vec3 vnormal;

out vec4 fcolor;

struct Light {
	vec3  color;
	float ambient, diffuse, specular;
	vec3  normal;
};

uniform Light light;

void main() {
	float diffuse = dot(
		normalize(cross(vnormal, vec3(1.f,0.f,0.f))),
		-light.normal
	);
	if(diffuse < 0.f) {
		diffuse = 0.f;
	}
	fcolor = vec4(
		vcolor.rgb * light.color *
		(light.ambient + light.diffuse*diffuse),
		1.f
	);
}
