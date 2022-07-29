#ifndef LIBRARY_H
#define LIBRARY_H

#include <QMultiMap>
#include <QList>
#include <QDateTime>

#include "common.h"
#include "utilities.h"

class SAuthor
{
public:
    QString getName() const;
    int nTag;
    QString sFirstName;
    QString sLastName;
    QString sMiddleName;
    QString sNickName;
};

struct SBook
{
    qlonglong id;
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
    QString sKeywords;
    bool bDeleted;
    bool bReaded;
    SBook()
    {
        id = 0;
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
    ulong id; // id жанра в sqlite базе данных
    QString sName; // русское название жанра
    QString sNameEn; // английское название жанра
    ushort idParrentGenre;
    ushort nSort;
    QString sCode;
    SGenre()
    {
        sName = "";
        sNameEn = "";
        idParrentGenre = 0;
        nSort = 0;
        sCode = "";
    }
};

class Group
{
private:
    QString Name_;
    QString BlockedName_;
    uint Id_;
    bool Blocked_;
public:
    Group() {
        Name_ = ""; Id_ = 0; Blocked_ = false; BlockedName_ = "";
    };
    Group(const QString& Name, uint id, bool blocked, const QString& blockedName = "") :
        Name_(Name), Id_(id), BlockedName_(blockedName), Blocked_(blocked) { };

    void setName(QString Name) { Name_ = Name; };
    void setId(uint Id) { Id_ = Id; };
    void setBlocked(bool blocked) { Blocked_ = blocked; };
    QString getName() const { return Name_; };
    uint getId() const { return Id_; };
    bool isBlocked() const { return Blocked_; };
    void setBlockedName(QString BlockedName) {
        if (isBlocked())
            BlockedName_ = BlockedName;
    };
    QString getBlockedName() const { return BlockedName_; };
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
    uint uIdCurrentGroup;
    uint uIdCurrentBookForAuthor;
    uint uIdCurrentBookForSeria;
    uint uIdCurrentBookForGenre;
    uint uIdCurrentBookForGroup;
    QString sCurrentSearchingFilter;
    uint uIdCurrentTag;
    QString sCurrentBookLanguage;
    QHash<uint,SAuthor> mAuthors;
    QMultiHash<uint,uint> mAuthorBooksLink;
    QHash<uint,SBook> mBooks;
    QHash<uint,SSerial> mSerials;
    QHash<uint, Group> mGroups;
    QMultiHash<uint, uint> mGroupBooksLink;
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
        uIdCurrentGroup = 0;
        uIdCurrentBookForAuthor = 0;
        uIdCurrentBookForSeria = 0;
        uIdCurrentBookForGenre = 0;
        uIdCurrentBookForGroup = 0;
        uIdCurrentTag = 0;
    }
    void clear()
    {
        name = "";
        path = "";
        sInpx = "";
        bFirstAuthor = false;;
        bWoDeleted = false;;
        bLoaded = false;
        nCurrentTab = 0;
        uIdCurrentAuthor = 0;
        uIdCurrentSeria = 0;
        uIdCurrentGenre = 0;
        uIdCurrentGroup = 0;
        uIdCurrentBookForAuthor = 0;
        uIdCurrentBookForSeria = 0;
        uIdCurrentBookForGenre = 0;
        uIdCurrentBookForGroup = 0;
        uIdCurrentTag = 0;
        sCurrentBookLanguage = "";
        sCurrentSearchingFilter = "";
        mAuthors.clear();
        mAuthorBooksLink.clear();
        mBooks.clear();
        mSerials.clear();
        mGroups.clear();
        mGroupBooksLink.clear();
        vLaguages.clear();
    }
    void clearOnlyContainers()
    {
        mAuthors.clear();
        mAuthorBooksLink.clear();
        mBooks.clear();
        mSerials.clear();
        mGroups.clear();
        mGroupBooksLink.clear();
        vLaguages.clear();
    }
};

extern bool g_db_is_open;

extern SLib currentLib;
extern QMap<int,SLib> mLibs;
extern QMap <uint,SGenre> mGenre;

class LibrarySQLiteWorker {
public:
    LibrarySQLiteWorker();
    void loadBooksDataFromSQLiteToLibraryStructure(int idLibrary);
    void loadGenresFromSQLiteToLibraryStructure();
    static void UpdateLibs();
};
#endif // LIBRARY_H
