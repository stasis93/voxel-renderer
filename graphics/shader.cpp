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

    m_vertex_id = glCreateShader(GL_VERTEX_SHADER);
    m_fragment_id = glCreateShader(GL_FRAGMENT_SHADER);

    glShaderSource(m_vertex_id, 1, &vsTextPtr, NULL);
    glCompileShader(m_vertex_id);

    checkShaderStatus(m_vertex_id);

    glShaderSource(m_fragment_id, 1, &fsTextPtr, NULL);
    glCompileShader(m_fragment_id);

    checkShaderStatus(m_fragment_id);

    m_id = glCreateProgram();
    glAttachShader(m_id, m_vertex_id);
    glAttachShader(m_id, m_fragment_id);

    glLinkProgram(m_id);
    checkShaderProgramStatus(m_id);

    glDeleteShader(m_vertex_id);
    glDeleteShader(m_fragment_id);

    Utils::glCheckError();
}

Shader::~Shader()
{
    //PRINT_FUNC();
    glDetachShader(m_id, m_vertex_id);
    Utils::glCheckError();

    glDetachShader(m_id, m_fragment_id);
    Utils::glCheckError();

//    glDeleteShader(m_vertex_id);
//    Utils::glCheckError();

//    glDeleteShader(m_fragment_id);
//    Utils::glCheckError();

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
