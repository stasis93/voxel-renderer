#version 330 core

in vec3 texCoord;

uniform samplerCube skybox;

void main()
{
    gl_FragColor = texture(skybox, texCoord);
}
