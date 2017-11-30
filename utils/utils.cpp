#include "utils.h"

#include "glad/glad.h"
#include <fstream>
#include <sstream>

namespace Utils
{
    std::string getTextFromFile(const char * file)
    {
        std::string text{""};
        std::ifstream ifs;

        ifs.exceptions(std::fstream::failbit | std::fstream::badbit);

        try
        {
            ifs.open(file);
            std::stringstream ss;
            ss << ifs.rdbuf();
            text = ss.str();
            ifs.close();
        }
        catch (std::fstream::failure & ex)
        {
            std::cout << "Can't read file " << file << " " << ex.what();
        }

        return text;
    }

    GLenum glCheckError_(const char * file, const char * func, int line)
    {
        GLenum errorCode;
        while ((errorCode = glGetError()) != GL_NO_ERROR)
        {
            std::string error;
            switch (errorCode)
            {
                case GL_INVALID_ENUM:                  error = "INVALID_ENUM"; break;
                case GL_INVALID_VALUE:                 error = "INVALID_VALUE"; break;
                case GL_INVALID_OPERATION:             error = "INVALID_OPERATION"; break;
                //case GL_STACK_OVERFLOW:                error = "STACK_OVERFLOW"; break;
                //case GL_STACK_UNDERFLOW:               error = "STACK_UNDERFLOW"; break;
                case GL_OUT_OF_MEMORY:                 error = "OUT_OF_MEMORY"; break;
                case GL_INVALID_FRAMEBUFFER_OPERATION: error = "INVALID_FRAMEBUFFER_OPERATION"; break;
            }
            std::cout << error << " | " << file << " | " << func << " (" << line << ")" << std::endl;
        }
        return errorCode;
    }
}
