#version 330

layout (location = 0) in vec3 coord;

uniform mat4 proj_view;
uniform mat4 model;

void main()
{
    gl_Position = proj_view * model * vec4(coord, 1);
}
