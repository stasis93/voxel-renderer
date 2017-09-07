#version 330

in vec4 texCoord;

void main()
{
	gl_FragColor = vec4(texCoord.w / 128.0f, texCoord.w / 256.0f, texCoord.w / 512.0f, 1.0f);
	//gl_FragColor = vec4(1, 0, 1, 1);
}