#version 150

in  vec3 inPosition;
in  vec3 inNormal;
in vec2 inTexCoord;
out vec2 texCoord;
out vec3 normal;

// NY
uniform mat4 ModelToWorld;
uniform mat4 WorldToView;
uniform mat4 ViewToProjection;

void main(void)
{
	texCoord = inTexCoord;
	normal = mat3(WorldToView*ModelToWorld)*inNormal;
	
	gl_Position = ViewToProjection * WorldToView * ModelToWorld * vec4(inPosition, 1.0);
}
