#version 330

layout (location = 0) in vec4 aPos;
out vec4 texCoord;

uniform mat4 model;
uniform mat4 proj_view;
uniform float time;

const float pi = 3.1415926;
const float waterOffsCoeff = 2 * pi / 16;

void main()
{
    bool isWater = aPos.w < 0 ? aPos.w == -7 : aPos.w == 7;
    float offs_y = 0;
    if (isWater) {
        offs_y = -0.4 + 0.1 * (sin(time + aPos.x * waterOffsCoeff) +
                               cos(time + aPos.z * waterOffsCoeff));
    }

	gl_Position = proj_view * model * vec4(aPos.x, aPos.y + offs_y, aPos.z, 1);
	texCoord = aPos;
}
