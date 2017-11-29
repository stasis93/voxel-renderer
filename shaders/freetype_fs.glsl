#version 330

in vec2 texCoord;
uniform sampler2D tex;
uniform vec4 color;

void main()
{
    gl_FragColor = vec4(1, 1, 1, texture2D(tex, texCoord).r) * color;
}
