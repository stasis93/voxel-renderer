#version 330

in vec4 texCoord;
out vec4 color;
uniform sampler2D blockTexture;

const vec4 fog_color = vec4(0.3f, 0.5f, 0.4f, 1.0f);
const float fog_density = .00003;

void main()
{
    vec2 texPos;
    float texOffset;

    if (texCoord.w > 0) // vertical faces
    {
        texOffset = texCoord.w - 1; // offset based on block type (w)
        texPos = vec2((fract(texCoord.x + texCoord.z) + texOffset) / 16.0f, texCoord.y);
    }
    else if (texCoord.w < 0) // horizontal faces
    {
        texOffset = -texCoord.w - 1;
        texPos = vec2((fract(texCoord.x) + texOffset) / 16.0f, texCoord.z);
    }

    color = texture(blockTexture, texPos);

    if (texCoord.w > 0) // simulate diffusion lighting
        color *= 0.6f;

    if (color.a < 0.5f)
        discard;

    float z = gl_FragCoord.z / gl_FragCoord.w;
    float fog = clamp(exp(-fog_density * z * z), 0.2, 1);
    color = mix(fog_color, color, fog);
}
