#include "shader.h"
#include <sstream>
#include <fstream>
#include <iostream>
#include "utils.h"


#include "glad/glad.h"
#include <string>


Shader::Shader(char const * vertexFilename, char const * fragmentFilename)
{
    //PRINT_FUNC();
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

    Utils::glCheckError();
}

Shader::~Shader()
{
    //PRINT_FUNC();
    glDeleteProgram(m_id);
    Utils::glCheckError();
}

void Shader::use()
{
    glUseProgram(m_id);
}

unsigned int Shader::id()
{
    return m_id;
}

void Shader::checkShaderStatus(unsigned int id)
{
    //PRINT_FUNC();
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
    //PRINT_FUNC();
    int success;
    char info[512];

    glGetProgramiv(id, GL_LINK_STATUS, &success);

    if (!success)
    {
        glGetProgramInfoLog(id, 512, NULL, info);
        std::cout << "ERROR::SHADERPROGRAM::LINK_FAILED\n" << info << std::endl;
    }
}
