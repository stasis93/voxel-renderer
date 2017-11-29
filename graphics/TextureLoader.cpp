#include "textureloader.h"
#include <glad/glad.h>
#include <iostream>
#include "stb_image.h"

TexturePtr TextureLoader::loadTexture(const std::string& path, bool flipVertically)
{
    GLuint id = 0;
    int w, h, ch;

    stbi_set_flip_vertically_on_load(flipVertically ? 1 : 0);
    unsigned char* data = stbi_load(path.c_str(), &w, &h, &ch, 4);
    if (!data)
    {
        std::cerr << "stbi_load failed for " << path << std::endl;
    }
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    stbi_image_free(data);

    return makeTexture(GL_TEXTURE_2D, id);
}

TexturePtr TextureLoader::loadCubeMap(const std::array<std::string, 6>& paths, bool flipVertically)
{
    GLuint id = 0;
    int w, h, ch;
    unsigned char* data = nullptr;

    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_CUBE_MAP, id);

    stbi_set_flip_vertically_on_load(flipVertically ? 1 : 0);

    for (int i = 0; i < 6; i++)
    {
        auto path = paths[i].c_str();
        data = stbi_load(path, &w, &h, &ch, 3);
        if (!data)
        {
            std::cerr << "stbi_load failed for " << path << std::endl;
        }
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, w, h, 0,
                     GL_RGB, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    /*
    #define GL_TEXTURE_CUBE_MAP_POSITIVE_X 0x8515
    #define GL_TEXTURE_CUBE_MAP_NEGATIVE_X 0x8516
    #define GL_TEXTURE_CUBE_MAP_POSITIVE_Y 0x8517
    #define GL_TEXTURE_CUBE_MAP_NEGATIVE_Y 0x8518
    #define GL_TEXTURE_CUBE_MAP_POSITIVE_Z 0x8519
    #define GL_TEXTURE_CUBE_MAP_NEGATIVE_Z 0x851A
    */

    return makeTexture(GL_TEXTURE_CUBE_MAP, id);
}

TexturePtr TextureLoader::makeTexture(unsigned int target, unsigned int id)
{
    TexturePtr t = std::make_unique<Texture>();
    t->m_id = id;
    t->m_target = target;
    return t;
}

