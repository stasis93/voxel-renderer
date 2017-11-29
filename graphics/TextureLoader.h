#ifndef TEXTURELOADER_H
#define TEXTURELOADER_H

#include <string>
#include <array>
#include <memory>

#include "texture.h"

using TexturePtr = std::unique_ptr<Texture>;

class TextureLoader
{
public:
    static TexturePtr loadTexture(const std::string& path, bool flipVertically = true);
    static TexturePtr loadCubeMap(const std::array<std::string, 6>& paths, bool flipVertically = true);

private:
    TextureLoader() = default;

    static TexturePtr makeTexture(unsigned int target, unsigned int id);
};

#endif // TEXTURELOADER_H
