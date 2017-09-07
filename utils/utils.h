#ifndef UTILS_H_INCLUDED
#define UTILS_H_INCLUDED

#include <string>
#include <iostream>

#define glCheckError() glCheckError_(__FILE__, __FUNCTION__, __LINE__)
//#define PRINT_FUNC() (std::cout << __FUNCTION__ << " (" << __FILE__ << ")" << std::endl)
#define PRINT_FUNC() (std::cout << __FUNCTION__ << std::endl)

using GLenum = unsigned int;

namespace Utils
{
    std::string getTextFromFile(const char * file);
    GLenum glCheckError_(const char * file, const char * func, int line);
}

#endif // UTILS_H_INCLUDED
