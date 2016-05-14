#version 150

out vec4 outColor;
in vec2 texCoord;
in vec3 normal;

uniform sampler2D tex;

const float ambient = 1;

void main(void)
{
	outColor = ambient * texture(tex, texCoord);
} 
