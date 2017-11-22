#version 330

layout (location = 0) in vec4 aPos;
out vec4 texCoord;

uniform mat4 model;
uniform mat4 proj_view;

void main()
{
	gl_Position = proj_view * model * vec4(aPos.xyz, 1);
	texCoord = aPos;
}
