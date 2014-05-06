#version 130 

in vec3 fs_color;
in vec3 fs_normal;
in vec3 fs_light;
in vec3 fs_eye;

out vec4 out_Color;

void main() {
    vec3 ambientContrib = fs_color * 0.1;
	vec4 diffuseColor = vec4(fs_color, 1.0);
	vec4 specColor = vec4(1, 1, 1, 1);

	//TODO: complete this using the diffuse equation from class!
	float diffuseTerm = dot(fs_normal, fs_light) / (length(fs_normal) * length(fs_light));
	diffuseTerm = clamp(diffuseTerm, 0, 0.9);
	
	// Calculate the half-way vector H = (L+V)/length(L+V)
	vec3 L = fs_light;
	vec3 V = fs_eye;
	vec3 H = normalize((L + V) / length(L + V));
	
	//Specular component
	float specContrib = pow(max(dot(H, fs_normal), 0), 64);
	
	//TODO: replace above out_Color with below once the diffuse term works.
    out_Color = diffuseTerm * diffuseColor + ambientContrib + specContrib*specColor;
}