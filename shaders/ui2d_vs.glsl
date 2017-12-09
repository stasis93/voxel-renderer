#version 330

layout (location = 0) in vec4 coord; // xy - position; zw - texture coords
out vec2 texCoord;

uniform mat4 projection;

void main()
{
    gl_Position = projection * vec4(coord.xy, 0, 1);
    texCoord = coord.zw;
}
