#ifndef FTLIBRARY_H
#define FTLIBRARY_H

#include "../utils/noncopyable.h"

class FT_LibraryRec_;
using FT_Library = FT_LibraryRec_*;


class FTLibrary : NonCopyable
{
public:
    ~FTLibrary();
    static FT_Library get();

private:
    FTLibrary();
    FT_Library m_ftLibrary;
};

#endif // FTLIBRARY_H
