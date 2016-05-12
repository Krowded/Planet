#version 150

out vec4 outColor;
in vec2 texCoord;
in vec3 normal;
uniform sampler2D tex;

uniform mat4 ModelToWorld;
uniform mat4 WorldToView;

const float ambient = 0.2;
const float diffuseConst = 0.5;

void main(void)
{
	vec3 lightDirection = normalize(mat3(WorldToView)*vec3(1.0, -1.0, 0.0));

	float diffuse = diffuseConst * max(dot(normalize(normal), lightDirection), 0);


	outColor = ( diffuse + ambient ) * texture(tex, texCoord);
} 
