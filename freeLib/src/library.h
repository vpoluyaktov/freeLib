#ifndef LIBRARY_H
#define LIBRARY_H

#include <QMultiMap>
#include <QList>
#include <QDateTime>
#include "common.h"

class SAuthor
{
public:
    QString getName() const;
    int nTag;
    QString sFirstName;
    QString sLastName;
    QString sMiddleName;
};

struct SBook
{
    QString sName;
    QString sAnnotation;
    QString sImg;
    QString sArchive;
    QString sIsbn;
    QDate date;
    QString sFormat;
    QList<uint> listIdGenres;
    QList<uint> listIdAuthors;
    uint idInLib;
    QString sFile;
    uint idSerial;
    uint idFirstAuthor;
    uint numInSerial;
    uint nSize;
    uchar nStars;
    uchar idLanguage;
    uchar nTag;
    bool bDeleted;
    bool bReaded;
    SBook()
    {
        idInLib = 0;
        idSerial = 0;
        idFirstAuthor = 0;
        numInSerial = 0;
        nSize = 0;
        nStars = 0;
        idLanguage = 0;
        nTag = 0;
        bDeleted = false;
        bReaded = false;
    }
};

struct SSerial
{
    QString sName;
    uchar nTag;
    SSerial()
    {
        nTag = 0;
    }
};

struct SGenre
{
    QString sName;
    ushort idParrentGenre;
    ushort nSort;
    SGenre()
    {
        idParrentGenre = 0;
        nSort = 0;
    }
};

struct SLib
{
    QString name;
    QString path;
    QString sInpx;
    bool bFirstAuthor;
    bool bWoDeleted;
    bool bLoaded;
    int nCurrentTab;
    uint uIdCurrentAuthor;
    uint uIdCurrentSeria;
    uint uIdCurrentGenre;
    uint uIdCurrentBookForAuthor;
    uint uIdCurrentBookForSeria;
    uint uIdCurrentBookForGenre;
    QString sCurrentSearchingFilter;
    uint uCurrentTag;
    QString sCurrentBookLanguage;
    QHash<uint,SAuthor> mAuthors;
    QMultiHash<uint,uint> mAuthorBooksLink;
    QHash<uint,SBook> mBooks;
    QHash<uint,SSerial> mSerials;
    QVector<QString> vLaguages;
    SLib()
    {
        bFirstAuthor = false;
        bWoDeleted = false;
        bLoaded = false;
        nCurrentTab = 0;
        uIdCurrentAuthor = 0;
        uIdCurrentSeria = 0;
        uIdCurrentGenre = 0;
        uIdCurrentBookForAuthor = 0;
        uIdCurrentBookForSeria = 0;
        uIdCurrentBookForGenre = 0;
        uCurrentTag = 0;
    }
};

void loadLibrary(uint idLibrary);
void loadGenres();

extern bool db_is_open;

extern SLib currentLib;
extern QMap<int,SLib> mLibs;
extern QMap <uint,SGenre> mGenre;

#endif // LIBRARY_H
