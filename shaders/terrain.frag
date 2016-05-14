#version 150

out vec4 outColor;
in vec2 texCoord;
in vec3 normal;

uniform sampler2D tex;

uniform mat4 ModelToWorld;
uniform mat4 WorldToView;

uniform vec3 SunlightDirection;

const float ambient = 0.1;
const float diffuseConst = 0.9;

void main(void)
{
	//vec3 lightDirection = normalize(mat3(WorldToView)*vec3(1.0, -1.0, 0.0));

	float diffuse = 0;//diffuseConst * max(dot(normalize(normal), lightDirection), 0);

	diffuse += diffuseConst * max(dot(normalize(normal), normalize(mat3(WorldToView)*SunlightDirection)), 0);

	outColor = min(diffuse + ambient, 1) * texture(tex, texCoord);
} 
