#include "constants.h"

namespace Consts
{
    const unsigned int  WINDOW_WIDTH = 1280,
                        WINDOW_HEIGHT = 720;

    const char * const WINDOW_NAME = "Voxel rendering!";

    const unsigned int FRAMERATE_LIMIT = 60;
    const float FIXED_TIMESTEP = 1.0f / 60;
}

namespace ShaderFiles
{
    char const * const vertex_shader_default = "shaders/default_vs.glsl";
    char const * const fragment_shader_default = "shaders/default_fs.glsl";

    char const * const vertex_shader_chunk = "shaders/chunk_vs.glsl";
    char const * const fragment_shader_chunk = "shaders/chunk_fs.glsl";

    char const * const vertex_shader_frustrum = "shaders/frustrum_vs.glsl";
    char const * const fragment_shader_frustrum = "shaders/frustrum_fs.glsl";
}
