#version 330

in vec4 texCoord;
out vec4 color;
uniform sampler2D blockTexture;

const vec4 fog_color = vec4(0.1f, 0.2f, 0.4f, 1.0f);
const float fog_density = .00003;

void main()
{
    vec2 texPos;

    if (texCoord.w > 0)
        texPos = vec2((fract(texCoord.x + texCoord.z) + (texCoord.w)) / 5.0f, texCoord.y);
    else
        texPos = vec2((fract(texCoord.x) - (texCoord.w)) / 5.0f, texCoord.z);

    color = texture(blockTexture, texPos);

    if (texCoord.w > 0)
        color *= 0.65f;

    if (color.a < 0.5f)
        discard;

    float z = gl_FragCoord.z / gl_FragCoord.w;
    float fog = clamp(exp(-fog_density * z * z), 0.2, 1);
    color = mix(fog_color, color, fog);
}
