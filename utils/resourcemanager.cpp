#include "resourcemanager.h"

TextureHolder& ResourceManager::textures()
{
    static TextureHolder th;
    return th;
}

ShaderHolder& ResourceManager::shaders()
{
    static ShaderHolder sh;
    return sh;
}
