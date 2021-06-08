#ifndef IMPORTTHREAD_H
#define IMPORTTHREAD_H

#include <QThread>
#include <QSqlQuery>
#include <QStringList>

//способы обновления
#define UT_FULL 10
#define UT_DEL_AND_NEW 11
#define UT_NEW 12



void ClearLib(QSqlDatabase dbase, qlonglong id_lib, bool delete_only);


class ImportThread : public QObject
{
    Q_OBJECT
public:
    explicit ImportThread(QObject *parent = 0);
    void start(
        const QString& fileName, const QString& name, const QString& path, long ID, int update_type,
        bool save_only = false, bool firstAuthor = false, bool bWoDeleted = false);
    //void SaveLibrary();
    bool loop_;

signals:
    void Message(const QString& str);
    void End();

public slots:
    void process();
    void break_import();

protected:
    ulong importBooksToLibrary(const QString& path);
    ulong importBooks(const QString& path,int &count);

    bool readFB2_FBD(const QByteArray &ba, const QString& file_name, const QString& arh_name, qint32 file_size = 0);
    bool readEPUB(const QByteArray &ba, const QString& file_name, const QString& arh_name, qint32 file_size = 0);
    void readBook(
        const QString& type, const QString& format, const QByteArray& ba,
        const QString& file_name, const QString& arh_name, qint32 file_size = 0
    );
private:
    QString InpxFileName_;
    QString LibName_;
    QString LibPath_;
    bool SaveOnly_;
    int UpdateType_;
    bool FirstAuthorOnly_;
    bool bWoDeleted_;
    long ExistingLibID_;
    QSqlQuery *Query_;

    qlonglong AddSeriaToSQLite(qlonglong libID, const QString& str, int tag);
    qlonglong AddAuthorToSQLite(qlonglong libID, const QString& str, qlonglong id_book, bool first_author, const QString& language, int tag);
    qlonglong AddGenreToSQLite(qlonglong id_lib, QString genre, qlonglong id_book, const QString& language);
    //qlonglong AddGroupToSQLite(qlonglong libID, const QString& group, qlonglong bookID);
    qlonglong AddBookToSQLite(
        qlonglong id_lib, qlonglong star, const QString& name, qlonglong id_seria, int num_in_seria,
        const QString& file, int size, int IDinLib, bool deleted, const QString& format, const QDate& date,
        const QString& language, const QString& keys, const QString& archive, int tag, bool readed
    );

    // id жанра 'Неотсортированное'
    qlonglong GetOtherGenreId() const;
};

#endif // IMPORTTHREAD_H
