#version 150

out vec4 out_Color;
in vec3 normal;
in vec2 texCoord;

uniform sampler2D texUnit;

void main(void)
{	
	out_Color = dot(normalize(normal), vec3(1.0, 1.0, 0.0)) * vec4(1,1,1,1);
}	