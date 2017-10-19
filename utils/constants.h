#ifndef CONSTANTS_H_INCLUDED
#define CONSTANTS_H_INCLUDED

namespace Consts
{
    extern const unsigned int   WINDOW_WIDTH,
                                WINDOW_HEIGHT;
    extern char const * const WINDOW_NAME;

    extern const unsigned int FRAMERATE_LIMIT;
    extern const float FIXED_TIMESTEP;
}

namespace ShaderFiles
{
    extern char const * const vertex_shader_default;
    extern char const * const fragment_shader_default;

    extern char const * const vertex_shader_chunk;
    extern char const * const fragment_shader_chunk;

    extern char const * const vertex_shader_frustrum;
    extern char const * const fragment_shader_frustrum;
}

#endif // CONSTANTS_H_INCLUDED
