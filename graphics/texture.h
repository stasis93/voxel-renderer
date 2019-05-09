#ifndef TEXTURE_H
#define TEXTURE_H

#include "utils/noncopyable.h"

class TextureLoader;

class Texture : NonCopyable
{
public:
    Texture() = default;
    ~Texture();

    void bind() const;
    unsigned int id() const {return m_id;}

private:
    unsigned int m_id {0};
    unsigned int m_target {0};

    friend class TextureLoader;
};

#endif // TEXTURE_H
