#include "FTLibrary.h"

#include <ft2build.h>
#include <iostream>
#include FT_FREETYPE_H


FTLibrary::FTLibrary()
{
    std::cout << __FUNCTION__ << std::endl;

    if (FT_Init_FreeType(&m_ftLibrary)) {
        std::cerr << "FT_Init_FreeType failed" << std::endl;
        exit(EXIT_FAILURE);
    }
}

FTLibrary::~FTLibrary()
{
    std::cout << __FUNCTION__ << std::endl;

    FT_Done_FreeType(m_ftLibrary);
}

FT_Library FTLibrary::get()
{ /* "lazy" initialization */
    static FTLibrary instance;
    return instance.m_ftLibrary;
}
