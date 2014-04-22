#version 130 

in vec3 fs_color;
in vec3 fs_light;
in vec3 fs_normal;

out vec4 out_Color;

void main() {
    vec3 ambientContrib = fs_color * 0.1;
	vec4 diffuseColor = vec4(fs_color, 1.0);

	//TODO: complete this using the diffuse equation from class!
	float diffuseTerm = max(0,dot(fs_normal,fs_light));

	//out_Color = diffuseColor;
	
	//TODO: replace above out_Color with below once the diffuse term works.
    out_Color = diffuseTerm * diffuseColor + ambientContrib;
}