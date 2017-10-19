#ifndef SHADER_H_INCLUDED
#define SHADER_H_INCLUDED

class Shader
{
public:
    Shader(char const * vertexFilename, char const * fragmentFilename);
    ~Shader();

    void use();
    unsigned int id();

private:
    void checkShaderStatus(unsigned int id);
    void checkShaderProgramStatus(unsigned int id);

private:
    unsigned int m_id; // Shader program id
    unsigned int m_vertex_id;
    unsigned int m_fragment_id;
};


#endif // SHADER_H_INCLUDED
