#version 150

out vec4 outColor;
in vec2 texCoord;
in vec3 normal;
uniform sampler2D tex;
uniform mat4 mdlMatrix;

void main(void)
{
	vec3 light = mat3(mdlMatrix) * vec3(1.0 , 1.0, 0.0);
	outColor = dot(normalize(normal), light) * texture(tex, texCoord);
} 
