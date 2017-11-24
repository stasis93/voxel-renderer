#include "constants.h"

namespace Consts
{
    const float FIXED_TIMESTEP = 1.0f / 60;
}

namespace ShaderFiles
{
    char const * const vertex_shader_default = "shaders/default_vs.glsl";
    char const * const fragment_shader_default = "shaders/default_fs.glsl";

    char const * const vertex_shader_chunk = "shaders/chunk_vs.glsl";
    char const * const fragment_shader_chunk = "shaders/chunk_fs.glsl";

    char const * const vertex_shader_skybox = "shaders/skybox_vs.glsl";
    char const * const fragment_shader_skybox = "shaders/skybox_fs.glsl";
}
