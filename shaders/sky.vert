#version 150

in  vec3 inPosition;
in  vec2 inTexCoord;

out vec2 texCoord;

uniform mat4 mvp;

void main(void)
{
	gl_Position = mvp * vec4(inPosition, 1.0);

	texCoord = inTexCoord;
}