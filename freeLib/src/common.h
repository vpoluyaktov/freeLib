#ifndef COMMON_H
#define COMMON_H

#include <QSettings>
#include <QtWidgets/QFileDialog>
#include <QDesktopServices>
#include <QSqlQuery>
#include <QDebug>
#include <QtWidgets/QLabel>
#include <QPixmap>
#include <QSqlError>
#include <QtWidgets/QMessageBox>
#include <QBuffer>
#include <QPixmap>
#include <QDateTime>
#include <QTranslator>
#include <QApplication>
#include <QCommandLineParser>
#include <QMultiMap>
#include <QList>

#include "quazip/quazip/quazip.h"

#define AppName  QString("freeLib")
#define OrgName  QString("freeLibOrg")
#define default_exp_file_name "%a/%s/%n3%b"
#define default_book_title "(%abbrs %n2) %b"
#define default_author_name "%nf %nm %nl"
#define default_cover_label "%abbrs - %n2"
#define default_OPDS_port  8080
#define default_proxy_port 8080
#define default_dropcaps_font "sangha.ttf"

extern QApplication* app;
extern QTranslator* translator;
extern QTranslator* translator_qt;
extern QCommandLineParser CMDparser;
extern int g_idCurrentLib;

const QString UnknownAuthor = "Unknown Author";
const QString WithoutGenre = "Without Genre";
const QString WithoutSeries = "{ " + QObject::tr("Without Series") + " }";
const QString WithoutTitle = "[ " + QObject::tr("Without Title") + " ]";

enum APP_MODE { MODE_LIBRARY, MODE_CONVERTER, MODE_SHELF };
enum SendType { ST_Device, ST_Mail };
SendType SetCurrentExportSettings(int index);

struct genre_info
{
    genre_info(QString genre, qlonglong id) :genre(genre), id(id)
    {
    }
    QString genre;
    qlonglong id;
};
struct author_info
{
    author_info(QString author = UnknownAuthor, qlonglong id = -1) : author(author), id(id)
    {
        QStringList sl = author.split(",");
        if (sl.count() > 0)
            lastname = sl[0].trimmed();
        if (sl.count() > 1)
            firstname = sl[1].trimmed();
        if (sl.count() > 2)
            middlename = sl[2].trimmed();
        if (sl.count() > 3)
            nickname = sl[3].trimmed();
    }
    author_info(QStringList authorInfo, qlonglong id) : id(id)
    {
        if (authorInfo.count() > 0)
            lastname = authorInfo[0].trimmed();
        if (authorInfo.count() > 1)
            firstname = authorInfo[1].trimmed();
        if (authorInfo.count() > 2)
            middlename = authorInfo[2].trimmed();
        if (authorInfo.count() > 3)
            nickname = authorInfo[3].trimmed();
        int i = 0;
        foreach(QString str, authorInfo) {
            author += str + ",";
            i++;
        }
        author = author.remove(author.length() - 1, 1);
    }
    QString author;
    qlonglong id;
    QString firstname;
    QString middlename;
    QString lastname;
    QString nickname;
};

struct seria_info
{
    seria_info(qlonglong id, QString seria, qlonglong number) : id(id), seria(seria), number(number)
    {
    }
    qlonglong id;
    QString seria;
    qlonglong number;
};

struct book_info
{
    qlonglong id;
    QString title;
    QString annotation;
    QList<author_info> authors;
    QList<genre_info> genres;
    QList<seria_info> serias;
    QString language;
    QString seria;
    QString isbn;
    QString keywords;
    qlonglong id_seria;
    int num_in_seria;
    QString img;
    int star;
    bool readed;
    book_info()
    {
        id = 0;
        id_seria = 0;
        num_in_seria = 0;
        star = 0;
        keywords = "";
        readed = false;
    }
    //    book_info(qlonglong id,qlonglong id_seria):id(id),id_seria(id_seria)
    //    {
    //        num_in_seria=0;
    //    }
};
struct tag
{
    QString name;
    QString css;
    QString font_name;
    quint16 font_size;
    tag(QString name, QString css, QString font_name, quint16 font_size) :name(name), css(css), font_name(font_name), font_size(font_size)
    {
    }
};
extern QList<tag> tag_list;


void GetBookInfo(
    book_info& bi, const QByteArray& data, QString type,
    bool& isBookWithoutTitle, bool& isAuthorWithoutData, bool& isSeriaWithoutName, bool& isGenreaWithoutName,
    bool info_only = false, uint id_book = 0
);
void SetLocale();
void ResetToDefaultSettings();
void setProxy();
bool openDB(bool create, bool replace);
bool SetCurrentZipFileName(QuaZip* zip, const QString& name);
QSettings* GetSettings(bool need_copy = false, bool reopen = false);
QString FindLocaleFile(QString locale, QString name, QString suffics);
QString Transliteration(QString str);
QString BuildFileName(QString filename);
QString decodeStr(const QString& str);
QString RelativeToAbsolutePath(QString path);

#endif // COMMON_H
