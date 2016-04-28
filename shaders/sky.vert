#version 150

in  vec3 inPosition;
in  vec3 inNormal;
in  vec2 inTexCoord;

out vec3 normal;
out vec2 texCoord;

uniform mat4 mdlMatrix;
uniform mat4 projMatrix;

void main(void)
{
	gl_Position = projMatrix * mdlMatrix * vec4(inPosition, 1.0);

	normal = mat3(mdlMatrix) * inNormal;

	texCoord = inTexCoord;
}


