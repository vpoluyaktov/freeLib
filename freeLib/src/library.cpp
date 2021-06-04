#include "common.h"
#include "library.h"

//SLib current_lib;
QMap<int,SLib> mLibs;
QMap <uint,SGenre> mGenre;

void loadBooksDataFromSQLiteToLibraryStructure(int idLibrary)
{
    if (!db_is_open)
        return;

    qint64 t_start = QDateTime::currentMSecsSinceEpoch();


    QSqlQuery query(QSqlDatabase::database("libdb"));
    query.setForwardOnly(true);

    SLib& lib = mLibs[idLibrary];
    lib.mSerials.clear();
    query.prepare("SELECT id, name, tag FROM seria WHERE id_lib=:id_lib;");
    //                    0    1     2
    query.bindValue(":id_lib",idLibrary);
    if (!query.exec())
        qDebug() << query.lastError().text();
    while (query.next()) {
        uint idSerial = query.value(0).toUInt();
        QString sName = query.value(1).toString();
        lib.mSerials[idSerial].sName = sName;
        lib.mSerials[idSerial].nTag = static_cast<uchar>(query.value(2).toUInt());
    }
    qint64 t_end = QDateTime::currentMSecsSinceEpoch();
    qDebug() << "loadSeria " << t_end-t_start << "msec";

    t_start = QDateTime::currentMSecsSinceEpoch();
    lib.mAuthors.insert(0, SAuthor());
    query.prepare("SELECT author.id, LastName, FirstName, MiddleName, NickName, author.tag FROM author WHERE id_lib=:id_lib;");
    //                          0       1          2           3          4          5
    query.bindValue(":id_lib", idLibrary);
    query.exec();
    while (query.next()) {
        uint idAuthor = query.value(0).toUInt();
        SAuthor &author = lib.mAuthors[idAuthor];
        author.sLastName = query.value(1).toString().trimmed();
        author.sFirstName = query.value(2).toString().trimmed();
        author.sMiddleName = query.value(3).toString().trimmed();
        author.sNickName = query.value(4).toString().trimmed();
        int nTag = query.value(5).toInt();
        lib.mAuthors[idAuthor].nTag = nTag;
    }
    t_end = QDateTime::currentMSecsSinceEpoch();
    qDebug() << "loadAuthor " << t_end-t_start << "msec";

    lib.mBooks.clear();
    query.setForwardOnly(true);
    query.prepare("SELECT id, name, star, id_seria, num_in_seria, language, file, size, deleted, date, format, id_inlib, archive, first_author_id, tag, readed, keys FROM book WHERE id_lib=:id_lib;");
    //                     0   1      2     3            4           5        6     7      8       9     10      11        12         13            14    15     16
    query.bindValue(":id_lib", idLibrary);
    if (!query.exec())
        qDebug() << query.lastError().text();
    while (query.next()) {
        QString sName = query.value(1).toString();
        uint id = query.value(0).toUInt();
        SBook &book = lib.mBooks[id];
        book.sName = sName;
        book.nStars = qvariant_cast<uchar>(query.value(2));
        book.idSerial = query.value(3).toUInt();
        book.numInSerial = query.value(4).toUInt();
        QString sLaguage = query.value(5).toString().toLower();
        int idLaguage = lib.vLaguages.indexOf(sLaguage);
        if (idLaguage < 0) {
            idLaguage = lib.vLaguages.count();
            lib.vLaguages << sLaguage;
        }
        book.idLanguage = static_cast<uchar>(idLaguage);
        book.sFile = query.value(6).toString();
        book.nSize = query.value(7).toUInt();
        book.bDeleted = query.value(8).toBool();
        book.date = query.value(9).toDate();
        book.sFormat = query.value(10).toString();
        book.idInLib = query.value(11).toUInt();
        book.sArchive = query.value(12).toString();
        book.idFirstAuthor = query.value(13).toUInt();
        book.nTag = qvariant_cast<uchar>(query.value(14));
        book.bReaded = query.value(15).toBool();
        book.sKeywords = query.value(16).toString();
    }

    lib.mAuthorBooksLink.clear();
    query.prepare("SELECT id_book, id_author FROM book_author WHERE id_lib=:id_lib;");
    //                       0        1
    query.bindValue(":id_lib",idLibrary);
    if (!query.exec())
        qDebug() << query.lastError().text();
    while (query.next()) {
        uint idBook = query.value(0).toUInt();
        uint idAuthor = query.value(1).toUInt();
        if (lib.mBooks.contains(idBook) && lib.mAuthors.contains(idAuthor)) {
            lib.mAuthorBooksLink.insert(idAuthor, idBook);
            lib.mBooks[idBook].listIdAuthors << idAuthor;
        }
    }
    auto iBook = lib.mBooks.begin();
    uint emptycount = 0;
    while (iBook != lib.mBooks.end()){
        if (iBook->listIdAuthors.isEmpty()) {
            iBook->listIdAuthors << 0;
            lib.mAuthorBooksLink.insert(0, iBook.key());
            emptycount++;
        }
        ++iBook;
    }

    // код Жанра Прочие/Неотсортированное
    query.prepare("SELECT id FROM genre WHERE name ='Неотсортированное';");
    if (!query.exec())
        qDebug() << query.lastError().text();
    query.next();
    uint idGenreUnsorted = query.value(0).toUInt();

    query.prepare("SELECT id_book, id_genre FROM book_genre WHERE id_lib=:id_lib;");
    //                       0        1
    query.bindValue(":id_lib", idLibrary);
    if (!query.exec())
        qDebug() << query.lastError().text();
    while (query.next()) {
        uint idBook = query.value(0).toUInt();
        uint idGenre = query.value(1).toUInt();
        if (idGenre == 0) idGenre = idGenreUnsorted;// 328; // Прочие/Неотсортированное
        if (lib.mBooks.contains(idBook))
            lib.mBooks[idBook].listIdGenres << idGenre;
    }
    lib.bLoaded = true;

    t_end = QDateTime::currentMSecsSinceEpoch();
    qDebug()<< "loadBooks " << t_end-t_start << "msec";

    loadGroupsFromSQLiteToLibraryStructure(g_idCurrentLib);
}

void loadGenresFromSQLiteToLibraryStructure()
{
    if (!db_is_open)
        return;
    qint64 t_start = QDateTime::currentMSecsSinceEpoch();
    QSqlQuery query(QSqlDatabase::database("libdb"));

    t_start = QDateTime::currentMSecsSinceEpoch();
    mGenre.clear();
    query.prepare("SELECT id, name, name_en, id_parent, sort_index, code FROM genre;");
    //                    0     1       2       3           4        5
    if (!query.exec())
        qDebug() << query.lastError().text();
    while (query.next()) {
        uint idGenre = query.value(0).toUInt();
        SGenre &genre = mGenre[idGenre];
        genre.sName = query.value(1).toString();
        genre.sNameEn = query.value(2).toString();
        genre.idParrentGenre = static_cast<ushort>(query.value(3).toUInt());
        genre.nSort = static_cast<ushort>(query.value(4).toUInt());
        genre.sCode = query.value(5).toString();
    }
    qint64 t_end = QDateTime::currentMSecsSinceEpoch();
    qDebug() << "loadGenre " << t_end-t_start << "msec";
}

void loadGroupsFromSQLiteToLibraryStructure(int idLibrary)
{
    if (!db_is_open)
        return;

    qint64 t_start = QDateTime::currentMSecsSinceEpoch();
    QSqlQuery query(QSqlDatabase::database("libdb"));
    query.setForwardOnly(true);

    SLib& lib = mLibs[idLibrary];
    lib.mGroups.clear();
    query.prepare("SELECT id, name, blocked, blocked_name FROM groups WHERE id_lib=:id_lib;");
    query.bindValue(":id_lib", idLibrary);
    if (!query.exec())
        qDebug() << query.lastError().text();

    QList<uint> idGroupList;
    while (query.next()) {
        uint idGroup = query.value(0).toUInt();
        QString sName = query.value(1).toString();
        bool isBlocked = query.value(2).toBool();
        QString sBlockedName = query.value(3).toString();
        lib.mGroups[idGroup].setId(idGroup);
        lib.mGroups[idGroup].setName(sName);
        lib.mGroups[idGroup].setBlocked(isBlocked);
        lib.mGroups[idGroup].setBlockedName(sBlockedName);
        idGroupList << idGroup;
    }

    foreach(uint idGroup, idGroupList) {
        query.prepare("SELECT book.id, book_group.group_id FROM book, book_group WHERE book_group.book_id = book.id AND book_group.id_lib = book.id_lib AND book_group.group_id = :idGroup AND book.id_lib = :id_lib;");
        query.bindValue(":id_lib", idLibrary);
        query.bindValue(":idGroup", idGroup);
        if (!query.exec())
            qDebug() << query.lastError().text();
        else {
            while (query.next()) {
                uint idBook = query.value(0).toUInt();
                uint idGroup = query.value(1).toUInt();
                if (lib.mBooks.contains(idBook) && lib.mGroups.contains(idGroup))
                    lib.mGroupBooksLink.insert(idGroup, idBook);
            }
        }
    }

    qint64 t_end = QDateTime::currentMSecsSinceEpoch();
    qDebug() << "loadGroupsFromSQLiteToLibraryStructure " << t_end - t_start << "msec";
}

QString SAuthor::getName() const
{
    QString sAuthorName = QString("%1 %2 %3 (%4)").arg(sLastName, sFirstName, sMiddleName, sNickName.trimmed()).trimmed();
    if (sAuthorName.indexOf("()") > 1) // только ФИО без nickname
        sAuthorName = sAuthorName.remove(sAuthorName.length() - 3, 3);
    else if (sAuthorName.length() > 3) // только nickname
        return sAuthorName;
    else // неизвестный автор - вообще без данных
        sAuthorName = QCoreApplication::translate("MainWindow", "Unknown Author");
    return sAuthorName;
}
