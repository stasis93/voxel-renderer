#ifndef FTLIBRARY_H
#define FTLIBRARY_H


class FT_LibraryRec_;
using FT_Library = FT_LibraryRec_*;


class FTLibrary
{
public:
    ~FTLibrary();
    static FT_Library get();

    FTLibrary  (const FTLibrary&) = delete;
    FTLibrary& operator=(const FTLibrary&) = delete;

private:
    FTLibrary();
    FT_Library m_ftLibrary;
};

#endif // FTLIBRARY_H
