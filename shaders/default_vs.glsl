#version 330

layout (location = 0) in vec3 aPos;

uniform mat4 mvp;

void main()
{
	gl_Position = mvp * vec4(aPos, 1);
}