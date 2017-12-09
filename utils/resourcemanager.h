#ifndef RESOURCEMANAGER_H
#define RESOURCEMANAGER_H

#include "noncopyable.h"
#include "resourceholder.h"
#include "shader.h"
#include "texture.h"

using ShaderHolder = ResourceHolder<Shader, std::string>;
using TextureHolder = ResourceHolder<Texture, std::string>;

class ResourceManager : NonCopyable
{
public:
    static TextureHolder& textures();
    static ShaderHolder& shaders();

private:
    ResourceManager() = default;
};

#endif // RESOURCEMANAGER_H
