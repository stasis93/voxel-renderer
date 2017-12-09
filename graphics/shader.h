#ifndef SHADER_H_INCLUDED
#define SHADER_H_INCLUDED

#include "glad/glad.h"
#include <string>

#include "noncopyable.h"

class Shader : NonCopyable
{
public:
    Shader() = default;
    Shader(char const * vertexFilename, char const * fragmentFilename);
    ~Shader();

    void load(char const * vertexFilename, char const * fragmentFilename);
    void use() const;
    unsigned int id() const;

    void setInt  (const std::string& name, const int value);
    void setFloat(const std::string& name, const float value);
    void setMat4(const std::string& name, const float* m);
    void setVec3(const std::string& name, const float* v);
    void setVec3(const std::string& name, float x, float y, float z);
    void setVec4(const std::string& name, const float* v);
    void setVec4(const std::string& name, float x, float y, float z, float w);

private:
    void cleanUp();
    void checkShaderStatus(unsigned int id);
    void checkShaderProgramStatus(unsigned int id);

private:
    unsigned int m_id {0};
};

#endif // SHADER_H_INCLUDED
