#ifndef SHADER_H_INCLUDED
#define SHADER_H_INCLUDED

#include "glad/glad.h"
#include <string>


class Shader
{
public:
    Shader() = default;
    ~Shader();

    void load(char const * vertexFilename, char const * fragmentFilename);
    void use() const;
    unsigned int id() const;

    enum class UniformType {Float, Int};

    template<typename T>
    void setUniform(UniformType t, const std::string& name, T val);

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

template<typename T>
void Shader::setUniform(UniformType t, const std::string& name, T val)
{
    switch (t)
    {
    case UniformType::Float:
        glUniform1f(glGetUniformLocation(m_id, name.c_str()), (float)val);
        break;
    case UniformType::Int:
        glUniform1i(glGetUniformLocation(m_id, name.c_str()), (int)val);
        break;
    default:
        break;
    }
}

#endif // SHADER_H_INCLUDED
