#include "shader.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include "utils/utils.h"


Shader::Shader(char const* vertexFilename, char const* fragmentFilename)
{
    load(vertexFilename, fragmentFilename);
}

void Shader::load(char const* vertexFilename, char const* fragmentFilename)
{
    if (m_id != 0)
        cleanUp();

    std::string vsText, fsText;

    vsText = Utils::getTextFromFile(vertexFilename);
    fsText = Utils::getTextFromFile(fragmentFilename);

    char const * vsTextPtr = vsText.c_str();
    char const * fsTextPtr = fsText.c_str();

    unsigned int vertex_id = glCreateShader(GL_VERTEX_SHADER);
    unsigned int fragment_id = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(vertex_id, 1, &vsTextPtr, NULL);
    glCompileShader(vertex_id);

    checkShaderStatus(vertex_id);

    glShaderSource(fragment_id, 1, &fsTextPtr, NULL);
    glCompileShader(fragment_id);

    checkShaderStatus(fragment_id);

    m_id = glCreateProgram();
    glAttachShader(m_id, vertex_id);
    glAttachShader(m_id, fragment_id);

    glLinkProgram(m_id);
    checkShaderProgramStatus(m_id);

    glDeleteShader(vertex_id);
    glDeleteShader(fragment_id);
}

Shader::~Shader()
{
    cleanUp();
}

void Shader::cleanUp()
{
    glDeleteProgram(m_id);
}

void Shader::use() const
{
    glUseProgram(m_id);
}

unsigned int Shader::id() const
{
    return m_id;
}

void Shader::checkShaderStatus(unsigned int id)
{
    int success;
    char info[512];

    glGetShaderiv(id, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        glGetShaderInfoLog(id, 512, NULL, info);
        std::cout << "ERROR::SHADER::COMPILATION_FAILED\n" << info << std::endl;
    }
}

void Shader::checkShaderProgramStatus(unsigned int id)
{
    int success;
    char info[512];

    glGetProgramiv(id, GL_LINK_STATUS, &success);

    if (!success)
    {
        glGetProgramInfoLog(id, 512, NULL, info);
        std::cout << "ERROR::SHADERPROGRAM::LINK_FAILED\n" << info << std::endl;
    }
}

void Shader::setInt(const std::string& name, const int value)
{
    glUniform1i(glGetUniformLocation(m_id, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, const float value)
{
    glUniform1f(glGetUniformLocation(m_id, name.c_str()), value);
}

void Shader::setMat4(const std::string& name, const float* m)
{
    glUniformMatrix4fv(glGetUniformLocation(m_id, name.c_str()), 1, GL_FALSE, m);
}

void Shader::setVec3(const std::string& name, const float* v)
{
    glUniform3fv(glGetUniformLocation(m_id, name.c_str()), 1, v);
}

void Shader::setVec4(const std::string& name, const float* v)
{
    glUniform4fv(glGetUniformLocation(m_id, name.c_str()), 1, v);
}

void Shader::setVec3(const std::string& name, float x, float y, float z)
{
    glUniform3f(glGetUniformLocation(m_id, name.c_str()), x, y, z);
}

void Shader::setVec4(const std::string& name, float x, float y, float z, float w)
{
    glUniform4f(glGetUniformLocation(m_id, name.c_str()), x, y, z, w);
}

