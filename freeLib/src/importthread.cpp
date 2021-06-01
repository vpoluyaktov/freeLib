#include <QDomDocument>
#include <QByteArray>
#include <QBuffer>

#include "importthread.h"
#include "quazip/quazip/quazip.h"
#include "quazip/quazip/quazipfile.h"
#include "common.h"
#include "library.h"

void ClearLib(QSqlDatabase dbase, qlonglong id_lib, bool delete_only)
{
    QSqlQuery query(dbase);
    if(delete_only)
    {
        query.exec("update book set deleted=1 where id_lib="+QString::number(id_lib));
    }
    else
    {
        query.exec("DELETE FROM book WHERE id_lib="+QString::number(id_lib));
        query.exec("DELETE FROM author WHERE id_lib="+QString::number(id_lib));
        query.exec("DELETE FROM seria WHERE id_lib="+QString::number(id_lib));
        query.exec("DELETE FROM groups WHERE id_lib=" + QString::number(id_lib));
        query.exec("DELETE FROM book_author WHERE id_lib="+QString::number(id_lib));
        query.exec("DELETE FROM book_genre WHERE id_lib=" + QString::number(id_lib));
        query.exec("DELETE FROM book_group WHERE id_lib="+QString::number(id_lib));
        query.exec("VACUUM");
    }
}

/*
    Масштаб обложки книги из настроек
*/
QSize GetCoverSize()
{
    QSettings settings;
    QSize picSize;
    switch (settings.value("CoverSize", 0).toInt())
    {
    case 0:
        picSize.setWidth(90);
        picSize.setHeight(120);
        break;
    case 1:
        picSize.setWidth(120);
        picSize.setHeight(160);
        break;
    case 2:
        picSize.setWidth(150);
        picSize.setHeight(200);
        break;
    case 3:
        picSize.setWidth(240);
        picSize.setHeight(320);
        break;
    case 4:
        picSize.setWidth(300);
        picSize.setHeight(400);
        break;
    case 5:
        picSize.setWidth(480);
        picSize.setHeight(640);
        break;
    case 6:
        picSize.setWidth(600);
        picSize.setHeight(800);
        break;
    case 7:
        return QSize();
        break;
    default:
        picSize.setWidth(300);
        picSize.setHeight(400);
        break;
    }
    return picSize;
}

void GetBookInfo(book_info &bi,const QByteArray &data,QString type,bool info_only,uint id_book)
{
    QSettings settings;
    bi.id=id_book;
    if(id_book==0 || !info_only)
    {
        if(type=="epub")
        {
            QBuffer buf;
            buf.setData(data);
            QuaZip zip(&buf);
            zip.open(QuaZip::mdUnzip);
            QBuffer info;
            SetCurrentZipFileName(&zip,"META-INF/container.xml");
            QuaZipFile zip_file(&zip);
            zip_file.open(QIODevice::ReadOnly);
            info.setData(zip_file.readAll());
            zip_file.close();
            QDomDocument doc;
            doc.setContent(info.data());
            QDomNode root=doc.documentElement();
            bool need_loop=true;
            QString rel_path;
            bi.num_in_seria=0;
            for(int i=0;i<root.childNodes().count() && need_loop;i++)
            {
                if(root.childNodes().at(i).nodeName().toLower()=="rootfiles")
                {
                    QDomNode roots=root.childNodes().at(i);
                    for(int j=0;j<roots.childNodes().count() && need_loop;j++)
                    {
                        if(roots.childNodes().at(j).nodeName().toLower()=="rootfile")
                        {
                            QString path=roots.childNodes().at(j).attributes().namedItem("full-path").toAttr().value();
                            QBuffer opf_buf;
                            QFileInfo fi(path);
                            rel_path=fi.path();
                            SetCurrentZipFileName(&zip,path);
                            zip_file.open(QIODevice::ReadOnly);
                            opf_buf.setData(zip_file.readAll());
                            zip_file.close();

                            QDomDocument opf;
                            opf.setContent(opf_buf.data());
                            QDomNode meta=opf.documentElement().namedItem("metadata");
                            for(int m=0;m<meta.childNodes().count();m++)
                            {
                                if(meta.childNodes().at(m).nodeName().right(5)=="title")
                                {
                                    bi.title=meta.childNodes().at(m).toElement().text().trimmed();
                                }
                                else if(meta.childNodes().at(m).nodeName().right(8)=="language")
                                {
                                    bi.language=meta.childNodes().at(m).toElement().text().trimmed();
                                }
                                else if(meta.childNodes().at(m).nodeName().right(7)=="creator")
                                {
                                    QStringList names=meta.childNodes().at(m).toElement().text().trimmed().split(" ");
                                    names.move(names.count()-1, 0);
                                    if (names.count() == 3)
                                        names.append(""); // NickName
                                    bi.authors << author_info(names, 0);
                                }
                                else if(meta.childNodes().at(m).nodeName().right(7)=="subject")
                                {
                                    QString  genre = meta.childNodes().at(m).toElement().text().trimmed();
                                    if(!genre.isEmpty())
                                        bi.genres<<genre_info(meta.childNodes().at(m).toElement().text().trimmed(),0);
                                }
                                else if(meta.childNodes().at(m).nodeName().right(11)=="description")
                                {
                                    QBuffer buff;
                                    buff.open(QIODevice::WriteOnly);
                                    QTextStream ts(&buff);
                                    ts.setCodec("UTF-8");
                                    meta.childNodes().at(m).save(ts,0,QDomNode::EncodingFromTextStream);
                                    bi.annotation=QString::fromUtf8(buff.data().data());
                                }
                                else if (meta.childNodes().at(m).nodeName().right(8) == "coverage")
                                {
                                    QString coverage = meta.childNodes().at(m).toElement().text().trimmed();
                                    if (!coverage.isEmpty())
                                        bi.keywords += !bi.keywords.isEmpty() ? (" , " + coverage) : coverage;
                                }
                                else if (meta.childNodes().at(m).nodeName().right(4) == "meta" && info_only)
                                {
                                    QString metaName = meta.childNodes().at(m).attributes().namedItem("name").toAttr().value();
                                    if (metaName == "calibre:title_sort") {
                                        if (bi.title == "")
                                            bi.title = meta.childNodes().at(m).attributes().namedItem("content").toAttr().value().trimmed();
                                    }
                                    else if (metaName == "calibre:series")
                                        bi.seria = meta.childNodes().at(m).attributes().namedItem("content").toAttr().value().trimmed();
                                    else if (metaName == "calibre:series_index")
                                        bi.num_in_seria = meta.childNodes().at(m).attributes().namedItem("content").toAttr().value().trimmed().toInt();
                                    else if (metaName == "keywords" || metaName == "keyword" || metaName == "keys") {
                                        QString keywords = meta.childNodes().at(m).attributes().namedItem("content").toAttr().value().trimmed();
                                        if (!keywords.isEmpty())
                                            bi.keywords += !bi.keywords.isEmpty() ? (" , " + keywords) : keywords;
                                    }
                                }
                                else if (meta.childNodes().at(m).nodeName().right(4)=="meta" && !info_only)
                                {
                                    if (meta.childNodes().at(m).attributes().namedItem("name").toAttr().value() == "cover") {
                                        QString cover = meta.childNodes().at(m).attributes().namedItem("content").toAttr().value();
                                        QDomNode manifest=opf.documentElement().namedItem("manifest");
                                        for(int man=0;man<manifest.childNodes().count();man++)
                                        {
                                            if(manifest.childNodes().at(man).attributes().namedItem("id").toAttr().value()==cover)
                                            {
                                                QBuffer img;
                                                cover=rel_path+"/"+manifest.childNodes().at(man).attributes().namedItem("href").toAttr().value();

                                                SetCurrentZipFileName(&zip,cover);
                                                zip_file.open(QIODevice::ReadOnly);
                                                img.setData(zip_file.readAll());
                                                zip_file.close();

                                                //проверить как работает
                                                QString sImgFile = QString("%1/freeLib/cover.jpg").arg(QStandardPaths::standardLocations(QStandardPaths::TempLocation).first());
                                                QPixmap image;
                                                image.loadFromData(img.data());
                                                // масштабирование обложки книги, если необходимо
                                                QSize picSize = GetCoverSize();
                                                if (picSize.width() != -1 && picSize.height() != -1)
                                                    image = image.scaled(picSize, Qt::KeepAspectRatio);
                                                image.save(sImgFile);

                                                //bi.img=("<td valign=top style=\"width:%1px\"><center><img src=\"data:"+manifest.childNodes().at(man).attributes().namedItem("media-type").toAttr().value()+
                                                //        ";base64,"+img.data().toBase64()+"\"></center></td>");
                                                bi.img=QString("<td valign=top style=\"width:1px\"><center><img src=\"file:%1\"></center></td>").arg(sImgFile);
                                                break;
                                            }
                                        }
                                    }
                                }
                            }
                            need_loop=false;
                        }
                    }
                }
            }
            zip.close();
            buf.close();
            info.close();
        }
        else if(type=="fb2" || type == "fbd")
        {
            QDomDocument doc;
            doc.setContent(data);
            QDomElement title_info=doc.elementsByTagName("title-info").at(0).toElement();
            if(!info_only)
            {
                QString cover=QString::fromStdString( title_info.elementsByTagName("coverpage").at(0).toElement().elementsByTagName("image").at(0).attributes().namedItem("l:href").toAttr().value().toStdString());
                if (cover == "")
                    cover = QString::fromStdString(title_info.elementsByTagName("coverpage").at(0).toElement().elementsByTagName("image").at(0).attributes().namedItem("xlink:href").toAttr().value().toStdString());
                if(cover.left(1)=="#")
                {
                    QDomNodeList binarys=doc.elementsByTagName("binary");
                    for(int i=0;i<binarys.count();i++)
                    {
                        if(binarys.at(i).attributes().namedItem("id").toAttr().value()==cover.right(cover.length()-1))
                        {
                            QString sImgFile = QString("%1/freeLib/cover.jpg").arg(QStandardPaths::standardLocations(QStandardPaths::TempLocation).first());
                            QPixmap image;
                            QByteArray ba;
                            ba.append(binarys.at(i).toElement().text());
                            QByteArray ba64 = QByteArray::fromBase64(ba);
                            image.loadFromData(ba64);

                            // масштабирование обложки книги, если необходимо
                            QSize picSize = GetCoverSize();
                            if (picSize.width() != -1 && picSize.height() != -1)
                                image = image.scaled(picSize, Qt::KeepAspectRatio);

                            image.save(sImgFile);
                            bi.img=QString("<td valign=top><center><img src=\"file:%1\"></center></td>").arg(sImgFile);
                            break;
                        }
                    }
                }
                QBuffer buff;
                buff.open(QIODevice::WriteOnly);
                QTextStream ts(&buff);
                ts.setCodec("UTF-8");
                title_info.elementsByTagName("annotation").at(0).save(ts,0,QDomNode::EncodingFromTextStream);
                bi.annotation=QString::fromUtf8(buff.data().data());
                bi.annotation.replace("<annotation>","",Qt::CaseInsensitive);
                bi.annotation.replace("</annotation>","",Qt::CaseInsensitive);
            }
            bi.title=title_info.elementsByTagName("book-title").at(0).toElement().text().trimmed();
            bi.keywords = title_info.elementsByTagName("keywords").at(0).toElement().text().trimmed();
            bi.language=title_info.elementsByTagName("lang").at(0).toElement().text();
            bi.seria=title_info.elementsByTagName("sequence").at(0).attributes().namedItem("name").toAttr().value().trimmed();
            bi.num_in_seria=title_info.elementsByTagName("sequence").at(0).attributes().namedItem("number").toAttr().value().trimmed().toInt();

            QDomNodeList serias = title_info.elementsByTagName("sequence");
            for (int i = 0; i < serias.count(); i++) {
                QString s = serias.at(i).attributes().namedItem("name").toAttr().value().trimmed();
                int n = serias.at(i).attributes().namedItem("number").toAttr().value().trimmed().toInt();
                if (!s.isEmpty())
                    bi.serias << seria_info(0, s, n);
            }

            QDomNodeList author=title_info.elementsByTagName("author");
            for(int i=0;i<author.count();i++)
            {
                author_info ti("",0);
                ti.id=0;
                ti.firstname=author.at(i).toElement().elementsByTagName("first-name").at(0).toElement().text();
                ti.lastname=author.at(i).toElement().elementsByTagName("last-name").at(0).toElement().text();
                ti.middlename=author.at(i).toElement().elementsByTagName("middle-name").at(0).toElement().text();
                ti.nickname = author.at(i).toElement().elementsByTagName("nickname").at(0).toElement().text();
                ti.author = ti.lastname + "," + ti.firstname + "," + ti.middlename + "," + ti.nickname;
                bi.authors<<ti;
            }

            QDomNodeList genre=title_info.elementsByTagName("genre");
            for(int i=0;i<genre.count();i++)
            {
                QString g = genre.at(i).toElement().text().trimmed();
                if(!g.isEmpty())
                    bi.genres<<genre_info(genre.at(i).toElement().text().trimmed(),0);
            }

            QDomElement publish_info=doc.elementsByTagName("publish-info").at(0).toElement();
            bi.isbn=publish_info.elementsByTagName("isbn").at(0).toElement().text();
        }
    }
    if(id_book > 0)
    {
        bi.authors.clear();
        bi.title = mLibs[g_idCurrentLib].mBooks[id_book].sName;
        bi.num_in_seria = mLibs[g_idCurrentLib].mBooks[id_book].numInSerial;
        bi.language = mLibs[g_idCurrentLib].vLaguages[mLibs[g_idCurrentLib].mBooks[id_book].idLanguage];
        bi.seria = mLibs[g_idCurrentLib].mSerials[mLibs[g_idCurrentLib].mBooks[id_book].idSerial].sName;
        bi.id_seria = mLibs[g_idCurrentLib].mBooks[id_book].idSerial;
        bi.readed = mLibs[g_idCurrentLib].mBooks[id_book].bReaded;
        bi.keywords = mLibs[g_idCurrentLib].mBooks[id_book].sKeywords;

        foreach (uint idAuthor,  mLibs[g_idCurrentLib].mBooks[id_book].listIdAuthors) {
            author_info ti("",0);
            ti.id = idAuthor;
            ti.author = mLibs[g_idCurrentLib].mAuthors[idAuthor].getName();
            bi.authors<<ti;
        }

        bi.genres.clear();
        foreach (uint idGenre, mLibs[g_idCurrentLib].mBooks[id_book].listIdGenres) {
            bi.genres << genre_info(mGenre[idGenre].sName.trimmed(), idGenre);
        }

    }

    if (bi.title.isEmpty())
        bi.title = "[ " + QObject::tr("Without Title") + " ]";
    if (bi.genres.count() == 0)
        bi.genres << genre_info("Without Genre", 0);
    if (bi.authors.count() == 0)
        bi.authors << author_info("", 0);
    if (bi.language.isEmpty())
        bi.language = "ru";
    if (bi.seria.isEmpty())
        bi.seria = "{ " + QObject::tr("Without Series") + " }";
}

ImportThread::ImportThread(QObject *parent) :
    QObject(parent)
{
}


//1-  Авторы через двоеточие
//2-  жанр
//3-  Название
//4-  Серия
//5-  Номер книги в серии
//6-  файл
//7-  размер
//8-  иденификатор
//9-  удален
//10- формат файла
//11- дата добавления
//12- язык
//13- рейтинг (0-5)
//14- ключевые слова



qlonglong ImportThread::AddSeriaToSQLite(QString str, qlonglong libID, int tag)
{
    if(str.trimmed().isEmpty())
        return -1;
    QString name=str.trimmed();
    Query_->exec("SELECT id FROM seria WHERE name='"+name+"' and id_lib="+QString::number(libID));
    if(Query_->next())
    {
        qlonglong id=Query_->value(0).toLongLong();
        return id;
    }
    Query_->prepare("INSERT INTO seria(name,id_lib,tag) values(:name,:id_lib,:tag)");
    Query_->bindValue(":name",name);
    Query_->bindValue(":id_lib",libID);
    Query_->bindValue(":tag",tag);
    if(!Query_->exec())
        qDebug() << Query_->lastError().text();
    qlonglong id = Query_->lastInsertId().toLongLong();
    return id;
}

qlonglong ImportThread::AddAuthorToSQLite(QString str, qlonglong libID, qlonglong id_book, bool first_author, QString language, int tag)
{
    if (str.trimmed().isEmpty())
        return -1;
    QStringList names=str.split(',');
    QString LastName;
    if (names.count() > 0)
        LastName=names[0].trimmed();
    QString FirstName;
    if (names.count() > 1)
        FirstName=names[1].trimmed();
    QString MiddleName;
    if (names.count() > 2)
        MiddleName=names[2].trimmed();
    QString NickName;
    if (names.count() > 3)
        NickName = names[3].trimmed();
    if (LastName.isEmpty() && FirstName.isEmpty() && MiddleName.isEmpty() && NickName.isEmpty())
        return -1;

    Query_->prepare("SELECT id, tag FROM author WHERE id_lib=:id_lib AND LastName=:LastName AND FirstName=:FirstName AND MiddleName=:MiddleName AND NickName=:NickName");
    Query_->bindValue(":id_lib", libID);
    Query_->bindValue(":LastName", LastName);
    Query_->bindValue(":FirstName", FirstName);
    Query_->bindValue(":MiddleName", MiddleName);
    Query_->bindValue(":NickName", NickName);
    if (!Query_->exec())
        qDebug() << Query_->lastError().text();
    qlonglong id = 0;
    if (Query_->next())
        id = Query_->value(0).toLongLong();
    if (id == 0) {
        Query_->prepare("INSERT INTO author(LastName,FirstName,MiddleName,NickName,id_lib,tag) VALUES(:LastName,:FirstName,:MiddleName,:NickName,:id_lib,:tag)");
        Query_->bindValue(":LastName", LastName);
        Query_->bindValue(":FirstName", FirstName);
        Query_->bindValue(":MiddleName", MiddleName);
        Query_->bindValue(":NickName", NickName);
        Query_->bindValue(":id_lib", libID);
        Query_->bindValue(":tag", tag);
        if(!Query_->exec())
            qDebug() << Query_->lastError().text();
        id = Query_->lastInsertId().toLongLong();
    }
    else {
        if (Query_->value(1).toInt() != tag && tag > 0)
            Query_->exec("UPDATE author SET tag=" + QString::number(tag) + " WHERE id=" + QString::number(id));
    }
    if(first_author)
        Query_->exec("UPDATE book SET first_author_id=" + QString::number(id) + " WHERE id=" + QString::number(id_book));
    Query_->exec("INSERT INTO book_author(id_book,id_author,id_lib,language) VALUES("+QString::number(id_book)+","+QString::number(id)+","+QString::number(libID)+",'"+language+"')");
    return id;
}

qlonglong ImportThread::AddBookToSQLite(
    qlonglong star, QString name, qlonglong id_seria, int num_in_seria, QString file,
    int size, int IDinLib, bool deleted, QString format, QDate date, QString language,
    QString keys, qlonglong id_lib, QString archive, int tag, bool readed
)
{
    Query_->prepare("INSERT INTO book(name,star,id_seria,num_in_seria,language,file,size,'deleted',date,keys,id_inlib,id_lib,format,archive,tag,readed) "
                   "values(:name,:star,:id_seria,:num_in_seria,:language,:file,:size,:deleted,:date,:keys,:id_inlib,:id_lib,:format,:archive,:tag,:readed)");

    Query_->bindValue(":name",name);
    Query_->bindValue(":star",star);
    Query_->bindValue(":id_seria",id_seria);
    Query_->bindValue(":num_in_seria",num_in_seria);
    Query_->bindValue(":language",language);
    Query_->bindValue(":file",file);
    Query_->bindValue(":size",size);
    Query_->bindValue(":deleted",deleted);
    Query_->bindValue(":date",date);
    Query_->bindValue(":keys",keys);
    Query_->bindValue(":id_inlib",IDinLib);
    Query_->bindValue(":id_lib",id_lib);
    Query_->bindValue(":format",format);
    Query_->bindValue(":archive",archive);
    Query_->bindValue(":tag",tag);
    Query_->bindValue(":readed", readed);
    if(!Query_->exec())
        qDebug() << Query_->lastError().text();
    qlonglong id = Query_->lastInsertId().toLongLong();

    return id;
}

qlonglong ImportThread::AddGenreToSQLite(qlonglong id_book, QString genre, qlonglong id_lib, QString language)
{
    qlonglong id_genre = 0;
    genre.replace(" ", "_");
    Query_->exec("SELECT id FROM genre WHERE code = '" + genre.toLower() + "'");
    if (Query_->next())
        id_genre = Query_->value(0).toLongLong();
    else {
        qDebug() << "Неизвестный жанр: " + genre;
        // код Жанра Прочие/Неотсортированное
        Query_->prepare("SELECT id FROM genre WHERE name ='Неотсортированное';");
        if (!Query_->exec())
            qDebug() << Query_->lastError().text();
        Query_->next();
        id_genre = Query_->value(0).toUInt();
    }
    Query_->exec(
        "INSERT INTO book_genre(id_book, id_genre, id_lib, language) VALUES(" +
        QString::number(id_book) + "," + QString::number(id_genre) + "," +
        QString::number(id_lib) + ",'" + language + "')"
    );
    Query_->exec("SELECT last_insert_rowid()");
    Query_->next();
    qlonglong id = Query_->value(0).toLongLong();
    return id;
}

qlonglong ImportThread::AddGroupToSQLite(qlonglong bookID, qlonglong libID, QString group)
{
    // проверка, есть ли в таблице groups добавляемая группа group для текущей библиотеки libID
    Query_->prepare("SELECT id FROM groups WHERE id_lib = :id_lib AND name = :group;");
    Query_->bindValue(":name", group);
    Query_->bindValue(":id_lib", libID);
    Query_->exec();
    qlonglong groupID = 0;
    if (Query_->next())
        groupID = Query_->value(0).toLongLong();
    if (groupID == 0)
    {
        // если группы group нет в таблице groups текущей библиотеки, то создаем запись
        Query_->prepare("INSERT INTO groups(name, id_lib) values(:name, :id_lib);");
        Query_->bindValue(":name", group);
        Query_->bindValue(":id_lib", libID);
        if (!Query_->exec())
            qDebug() << Query_->lastError().text();
        groupID = Query_->lastInsertId().toLongLong();
    }
    // заполнение таблицы book_group данными на группу group
    Query_->prepare("INSERT INTO book_group(id_book, id_group, id_lib) values(:id_book, :id_group, :id_lib);");
    Query_->bindValue(":id_book", bookID);
    Query_->bindValue(":id_group", groupID);
    Query_->bindValue(":id_lib", libID);
    if (!Query_->exec())
        qDebug() << Query_->lastError().text();
    qlonglong id = Query_->lastInsertId().toLongLong();
    return id;
}

void ImportThread::readFB2_FBD(const QByteArray& ba, QString file_name, QString arh_name, qint32 file_size)
{
    QFileInfo fi(file_name);
    Query_->exec(QString("SELECT id FROM book WHERE id_lib=%1 AND file='%2' AND archive='%3'").arg(QString::number(ExistingLibID_), file_name, arh_name));
    if (Query_->next()) { //если книга найдена, то просто снимаем пометку удаления
        Query_->exec("update book set deleted=0 where id=" + Query_->value(0).toString());
        return;
    }

    if (!arh_name.isEmpty()) { // zip
        int index = file_name.indexOf("/");
        if (index == -1)
            index = file_name.indexOf("\\");
        if (index != -1)
            file_name = file_name.mid(index+1, file_name.length());
    }
    QString message = QString(tr("add (%1):  %2")).arg(fi.suffix(), file_name);
    if (!arh_name.isEmpty()) // zip
        message += "  " + QString(tr("from zip:  %1")).arg(arh_name);
    emit Message(message);

    book_info bi;
    GetBookInfo(bi, ba, "fb2", true);
    qlonglong id_seria = AddSeriaToSQLite(bi.seria, ExistingLibID_, 0);
    qlonglong id_book = AddBookToSQLite(
        bi.star, bi.title, id_seria, bi.num_in_seria, file_name,
        (file_size == 0 ? ba.size() : file_size), 0, false, fi.suffix(), QDate::currentDate(),
        bi.language, bi.keywords, ExistingLibID_, arh_name, 0, bi.readed
    );

    bool first_author = true;
    foreach(author_info author, bi.authors) {
        AddAuthorToSQLite(author.author, ExistingLibID_, id_book, first_author, bi.language, 0);
        first_author = false;
        if(FirstAuthorOnly_)
            break;
    }
    foreach(genre_info genre, bi.genres)
        AddGenreToSQLite(id_book, genre.genre, ExistingLibID_, bi.language);
}

void ImportThread::readEPUB(const QByteArray &ba, QString file_name, QString arh_name, qint32 file_size)
{
    Query_->exec(QString("SELECT id FROM book where id_lib=%1 and file='%2' and archive='%3'").arg(QString::number(ExistingLibID_), file_name, arh_name));
    if(Query_->next()) { //если книга найдена, то просто снимаем пометку удаления
        Query_->exec("update book set deleted=0 where id=" + Query_->value(0).toString());
        return;
    }
    emit Message(tr("add (epub):") + " " + file_name);

    book_info bi;
    GetBookInfo(bi, ba, "epub", true);

    qlonglong id_seria = AddSeriaToSQLite(bi.seria, ExistingLibID_, 0);
    qlonglong id_book = AddBookToSQLite(
        bi.star, bi.title, id_seria, bi.num_in_seria, file_name,
        (file_size == 0 ? ba.size() : file_size), 0, false, "epub", QDate::currentDate(),
        bi.language, bi.keywords, ExistingLibID_, arh_name, 0, bi.readed
    );

    bool first_author = true;
    foreach(author_info author, bi.authors) {
        AddAuthorToSQLite(author.author, ExistingLibID_, id_book, first_author, bi.language, 0);
        first_author = false;
        if (FirstAuthorOnly_)
            break;
    }
    foreach(genre_info genre, bi.genres)
        AddGenreToSQLite(id_book, genre.genre, ExistingLibID_, bi.language);
}

void ImportThread::readFB2_test(const QByteArray& ba, QString file_name, QString arh_name)
{
    return;
    if (arh_name.isEmpty()) {
        file_name = file_name.right(file_name.length() - LibPath_.length());
        if (file_name.left(1) == "/" || file_name.left(1) == "\\")
            file_name = file_name.right(file_name.length() - 1);
    }
    else {
        arh_name = arh_name.right(arh_name.length() - LibPath_.length());
        if (arh_name.left(1) == "/" || arh_name.left(1) == "\\")
            arh_name = arh_name.right(arh_name.length() - 1);
    }
    file_name = file_name.left(file_name.length() - 4);
    if (Query_->next()) { //если книга найдена, то просто снимаем пометку удаления
        Query_->exec("update book set deleted=0 where id=" + Query_->value(0).toString());
        return;
    }
    QString title;
    title = "test";//title_info.elementsByTagName("book-title").at(0).toElement().text().trimmed();
    QString language = "ru";//title_info.elementsByTagName("lang").at(0).toElement().text();
    QString seria = "";//title_info.elementsByTagName("sequence").at(0).attributes().namedItem("name").toAttr().value().trimmed();
    QString num_seria = "0";//title_info.elementsByTagName("sequence").at(0).attributes().namedItem("number").toAttr().value().trimmed();


    qlonglong id_seria = AddSeriaToSQLite(seria, ExistingLibID_, 0);
    qlonglong id_book = AddBookToSQLite(
        0, title, id_seria, num_seria.toInt(), file_name, ba.size(), 0, false, "fb2",
        QDate::currentDate(), language, "", ExistingLibID_, arh_name, 0, 0
    );
    return;
    bool first_author = true;
    AddAuthorToSQLite("Иванов, Иван,Иванович ",
        ExistingLibID_, id_book, first_author, language, 0);
    first_author = false;
    AddGenreToSQLite(id_book, "sf", ExistingLibID_, language);
}

void ImportThread::importBooksToLibrary(QString path)
{
    int count=0;
    QStringList DirList = path.split("|");
    for (QString dirPath : DirList)
    {
        importBooks(dirPath, count);
        if (count > 0)
        {
            Query_->exec("COMMIT;");
            count = 0;
        }
    }
}

void ImportThread::importBooks(QString path, int &count)
{
    QDir dir(path);
    QFileInfoList info_list = dir.entryInfoList(QDir::NoSymLinks|QDir::NoDotAndDotDot|QDir::Readable|QDir::Files|QDir::Dirs);
    QList<QFileInfo>::iterator iter=info_list.begin();
    QString file_name;
    for(iter=info_list.begin();iter != info_list.end() && loop_;iter++)
    {
        app->processEvents();
        file_name = iter->absoluteFilePath();
        if(iter->isDir())
        {
            importBooks(file_name,count);
        }
        else
        {
            if(iter->suffix().toLower()!="fbd" &&
                    !(iter->suffix().toLower()=="zip" ||
                     iter->suffix().toLower()=="fb2" ||
                     iter->suffix().toLower()=="epub"))
            {
                QString fbd=iter->absolutePath()+"/"+iter->completeBaseName()+".fbd";
                QFile file(fbd);
                if(file.exists())
                {
                    file.open(QFile::ReadOnly);
                    readFB2_FBD(file.readAll(), file_name, "", iter->size());
                    count++;
                }
            }
            else if(iter->suffix().toLower()=="fb2" || iter->suffix().toLower()=="epub")
            {
                if(count==0)
                    Query_->exec("BEGIN;");
                QFile file(file_name);
                file.open(QFile::ReadOnly);
                QByteArray ba=file.readAll();
                if(iter->suffix().toLower()=="fb2")
                    readFB2_FBD(ba, file_name, "");
                else
                    readEPUB(ba,file_name,"");
                count++;
            }
            else if(iter->suffix().toLower()=="zip")
            {
                if(UpdateType_==UT_NEW) // Добавить новые книги
                {
                    //emit Message(tr("Read archive:") + " " + iter->fileName());
                    Query_->exec(QString("SELECT * FROM book where id_lib=%1 and archive='%2' LIMIT 1").arg(QString::number(ExistingLibID_), file_name/*arh_name*/));
                    if(Query_->next())
                        continue;
                }
                QuaZip uz(file_name);
                uz.setFileNameCodec(QTextCodec::codecForName("IBM 866"));
                if (!uz.open(QuaZip::mdUnzip))
                {
                    qDebug()<<("Error open archive!")<<" "<<file_name;
                    continue;
                }

                QuaZipFile zip_file(&uz);
                QList<QuaZipFileInfo64> list=uz.getFileInfoList64();
                foreach( QuaZipFileInfo64 str , list  )
                {
                    app->processEvents();
                    if(!loop_)
                        break;
                    QBuffer buffer;
                    if(count==0)
                        Query_->exec("BEGIN;");
                    QuaZipFileInfo zip_fi;
                    str.toQuaZipFileInfo(zip_fi);
                    if(zip_fi.name.right(3).toLower()=="fb2")
                    {
                        //uz.extractFile(str.filename,&buffer,UnZip::SkipPaths,16*1024);
                        SetCurrentZipFileName(&uz,zip_fi.name);
                        zip_file.open(QIODevice::ReadOnly);
                        buffer.setData(zip_file.read(16*1024));
                        zip_file.close();
                        readFB2_FBD(buffer.data(), str.name, file_name, str.uncompressedSize);
                    }
                    else if(zip_fi.name.right(3).toLower()=="epub")
                    {
                        SetCurrentZipFileName(&uz,zip_fi.name);
                        zip_file.open(QIODevice::ReadOnly);
                        buffer.setData(zip_file.readAll());
                        zip_file.close();
                        readEPUB(buffer.data(),str.name,file_name, str.uncompressedSize);
                    }
                    else if(zip_fi.name.right(3).toLower()!="fbd")
                    {
                        QFileInfo fi(str.name);
                        if(fi.completeBaseName().left(1)!="." && !fi.completeBaseName().isEmpty())
                        {
                            QString fbd = fi.path() != "."
                                ? fi.path() + "/" + fi.completeBaseName() + ".fbd"  /* файлы в zip в папке */
                                : fi.completeBaseName() + ".fbd";                   /* файлы в zip без папки */
                            // чтение данных описания книги из fbd файла
                            if(SetCurrentZipFileName(&uz, fbd))
                            {
                                zip_file.open(QIODevice::ReadOnly);
                                buffer.setData(zip_file.readAll());
                                zip_file.close();
                                readFB2_FBD(buffer.data(), str.name, file_name, str.uncompressedSize);
                            }
                        }
                    }

                    count++;
                    if(count==1000)
                    {
                        Query_->exec("COMMIT;");
                        count=0;
                    }
                }
            }
            if(count==1000)
            {
                Query_->exec("COMMIT;");
                count=0;
            }
        }
    }
}

void ImportThread::start(QString InpxFileName, QString LibName, QString LibPath, long LibID, int UpdateType, bool SaveOnly, bool FirstAuthorOnly, bool bWoDeleted)
{
    InpxFileName_ = RelativeToAbsolutePath(InpxFileName);
    if (!QFileInfo(InpxFileName_).exists() || !QFileInfo(InpxFileName_).isFile())
    {
        InpxFileName_ = InpxFileName;
    }
    LibName_ = LibName;
    LibPath_ = RelativeToAbsolutePath(LibPath);
    ExistingLibID_ = LibID;
    UpdateType_ = UpdateType;
    SaveOnly_ = SaveOnly;
    FirstAuthorOnly_ = FirstAuthorOnly;
    bWoDeleted_ = bWoDeleted;
    loop_ = true;
}

void ImportThread::process()
{
    if(SaveOnly_)
    {
        emit End();
        return;
    }
    if(LibName_.isEmpty())
    {
        emit Message(tr("Empty library name"));
        emit End();
        return;
    }
    QSettings *settings=GetSettings();
    QFileInfo fi(RelativeToAbsolutePath(settings->value("database_path").toString()));
    QString sDbFile=fi.canonicalFilePath();
    QSqlDatabase dbase = QSqlDatabase::addDatabase("QSQLITE","importdb");
    dbase.setDatabaseName(sDbFile);
    if (!dbase.open())
    {
        qDebug() << ("Error connect! ")<<sDbFile;
        return;
    }

    Query_=new QSqlQuery(dbase);

    switch(UpdateType_)
    {
    case UT_FULL: // Пересоздать библиотеку
        ClearLib(dbase,ExistingLibID_,false);
        break;
    case UT_DEL_AND_NEW: // Удалить несуществующие и добавить новые книги
        ClearLib(dbase,ExistingLibID_,true);
        break;
    }

    if(InpxFileName_.isEmpty())
    {
        importBooksToLibrary(LibPath_);
        Query_->exec("DROP TABLE IF EXISTS tmp;");
        Query_->exec(QString("CREATE TABLE tmp AS SELECT id FROM book WHERE id_lib=%1 AND deleted=1;").arg(QString::number(ExistingLibID_)));
        Query_->exec(QString("DELETE FROM book WHERE id_lib=%1 AND id IN (SELECT IN FROM tmp);").arg(QString::number(ExistingLibID_)));
        Query_->exec(QString("DELETE FROM book_genre WHERE id_lib=%1 AND id_book IN (SELECT id FROM tmp);").arg(QString::number(ExistingLibID_)));
        Query_->exec(QString("DELETE FROM book_author WHERE id_lib=%1 AND id_book IN (SELECT id FROM tmp);").arg(QString::number(ExistingLibID_)));
        Query_->exec(QString("DELETE FROM book_group WHERE id_lib=%1 AND id_book IN (SELECT id FROM tmp);").arg(QString::number(ExistingLibID_)));
        Query_->exec("DROP TABLE IF EXISTS tmp;");
        Query_->exec("VACUUM");
        emit End();
        return;
    }
    QuaZip archiveFile;
    QuaZip uz(InpxFileName_);
    if (!uz.open(QuaZip::mdUnzip))
    {
        qDebug()<<("Error open inpx!");
        emit End();
        return;
    }
    QStringList list = uz.getFileNameList();
    qlonglong book_count=0;
#define _NAME           0
#define _SERIA          1
#define _NUM_IN_SERIA   2
#define _FILE           3
#define _SIZE           4
#define _ID_IN_LIB      5
#define _DELETED        6
#define _FORMAT         7
#define _DATE           8
#define _LANGUAGE       9
#define _STAR           10
#define _KEYS           11
#define _AUTHORS        12
#define _GENRES         13
#define _FOLDER         14
#define _TAG            15
#define _TAG_SERIA      16
#define _TAG_AUTHOR     17

    int field_index[20];
    field_index[_NAME]          =2; //name
    field_index[_SERIA]         =3; //seria
    field_index[_NUM_IN_SERIA]  =4; //num_in_seria
    field_index[_FILE]          =5; //file
    field_index[_SIZE]          =6; //size
    field_index[_ID_IN_LIB]     =7; //id_in_lib
    field_index[_DELETED]       =8; //deleted
    field_index[_FORMAT]        =9; //format
    field_index[_DATE]          =10;//date
    field_index[_LANGUAGE]      =11;//language
    field_index[_STAR]          =12;//star
    field_index[_KEYS]          =13;//keys
    field_index[_AUTHORS]       =0; //Authors
    field_index[_GENRES]        =1; //Genres
    field_index[_FOLDER]        =-1; //Folder
    field_index[_TAG]           =-1; //Tag
    field_index[_TAG_SERIA]     =-1; //TagSeria
    field_index[_TAG_AUTHOR]    =-1; //TagAuthor

    QuaZipFile zip_file(&uz);
    foreach( QString str , list  )
    {
        if(QString(str).toUpper()=="STRUCTURE.INFO")
        {
            for(unsigned int i=0;i<sizeof(field_index)/sizeof(int);i++)
                field_index[i]=-1;
            QBuffer outbuff;
            SetCurrentZipFileName(&uz,str);
            zip_file.open(QIODevice::ReadOnly);
            outbuff.setData(zip_file.readAll());
            zip_file.close();
            QStringList lines=(QString::fromUtf8(outbuff.data())).split('\n');
            foreach(QString line,lines)
            {
                QStringList substrings=line.toUpper().split(';');
                int i=0;
                foreach(QString substring,substrings)
                {
                    if(substring=="TITLE")
                        field_index[_NAME]=i;
                    else if(substring=="SERIES")
                        field_index[_SERIA]=i;
                    else if(substring=="SERNO")
                        field_index[_NUM_IN_SERIA]=i;
                    else if(substring=="FILE")
                        field_index[_FILE]=i;
                    else if(substring=="SIZE")
                        field_index[_SIZE]=i;
                    else if(substring=="LIBID")
                        field_index[_ID_IN_LIB]=i;
                    else if(substring=="DEL")
                        field_index[_DELETED]=i;
                    else if(substring=="EXT")
                        field_index[_FORMAT]=i;
                    else if(substring=="DATE")
                        field_index[_DATE]=i;
                    else if(substring=="LANG")
                        field_index[_LANGUAGE]=i;
                    else if(substring=="STARS")
                        field_index[_STAR]=i;
                    else if(substring=="KEYWORDS")
                        field_index[_KEYS]=i;
                    else if(substring=="AUTHOR")
                        field_index[_AUTHORS]=i;
                    else if(substring=="GENRE")
                        field_index[_GENRES]=i;
                    else if(substring=="FOLDER")
                        field_index[_FOLDER]=i;
                    else if(substring=="TAG")
                        field_index[_TAG]=i;
                    else if(substring=="TAGSERIES")
                        field_index[_TAG_SERIA]=i;
                    else if(substring=="TAGAUTHOR")
                        field_index[_TAG_AUTHOR]=i;
                  i++;
                }

            }
            break;
        }
    }
    dbase.transaction();

    foreach( QString str , list  )
    {
        app->processEvents();
        if(!loop_)
        {
            break;
        }
        if(str.right(3).toUpper()!="INP")
            continue;
        emit Message(str);
        if(UpdateType_==UT_NEW) // Добавить новые книги
        {
            Query_->exec(QString("SELECT * FROM book where archive='%1' and id_lib=%2 LIMIT 1").arg(str,QString::number(ExistingLibID_)));
            if(Query_->next())
                continue;
        }
        QBuffer outbuff;
        SetCurrentZipFileName(&uz,str);
        zip_file.open(QIODevice::ReadOnly);
        outbuff.setData(zip_file.readAll());
        zip_file.close();
        //uz.extractFile(str,&outbuff,UnZip::SkipPaths);
        QStringList lines=(QString::fromUtf8(outbuff.data())).split('\n');
        qlonglong t1_=0,t2_=0,t3_=0,t4_=0,count=0;
        foreach(QString line,lines)
        {
            if(line.isEmpty())
                continue;

            qlonglong t0=QDateTime::currentMSecsSinceEpoch();
            app->processEvents();
            if(!loop_)
            {
                break;
            }
            QStringList substrings=line.split(QChar(4));
            if(substrings.count()==0)
                continue;
            QString name;
            if(substrings.count()>field_index[_NAME])
            {
                name=substrings[field_index[_NAME]].trimmed();
            }
            QString Seria;
            if(substrings.count()>field_index[_SERIA])
                Seria=substrings[field_index[_SERIA]].trimmed();
            int num_in_seria=0;
            if(substrings.count()>field_index[_NUM_IN_SERIA])
            {
                num_in_seria=substrings[field_index[_NUM_IN_SERIA]].trimmed().toInt();
            }
            QString file;
            if(substrings.count()>field_index[_FILE])
            {
                file=substrings[field_index[_FILE]].trimmed();
            }
            int size=0;
            if(substrings.count()>field_index[_SIZE])
            {
                size=substrings[field_index[_SIZE]].trimmed().toInt();
            }
            int id_in_lib=0;
            if(substrings.count()>field_index[_ID_IN_LIB])
            {
                id_in_lib=substrings[field_index[_ID_IN_LIB]].trimmed().toInt();
            }
            bool deleted=0;
            if(substrings.count()>field_index[_DELETED])
            {
                deleted=(substrings[field_index[_DELETED]].trimmed().toInt()>0);
            }
            QString format;
            if(substrings.count()>field_index[_FORMAT])
            {
                format=substrings[field_index[_FORMAT]].trimmed();
            }

            QDate date;
            if(substrings.count()>field_index[_DATE])
            {
                date=QDate::fromString(substrings[field_index[_DATE]].trimmed(),"yyyy-MM-dd");
            }
            QString language;
            if(substrings.count()>field_index[_LANGUAGE])
            {
                language=substrings[field_index[_LANGUAGE]].trimmed();
            }
            qlonglong star=0;
            if(substrings.count()>field_index[_STAR] && field_index[_STAR]>=0)
            {
                star=substrings[field_index[_STAR]].trimmed().toInt();
            }
            QString keys;
            if(substrings.count()>field_index[_KEYS])
            {
                keys=substrings[field_index[_KEYS]].trimmed();
            }
            QString folder=str;
            if(substrings.count()>field_index[_FOLDER] && field_index[_FOLDER]>=0)
            {
                folder=substrings[field_index[_FOLDER]].trimmed();
            }
            int tag=0;
            if(substrings.count()>field_index[_TAG] && field_index[_TAG]>=0)
            {
                tag=substrings[field_index[_TAG]].trimmed().toInt();
            }
            int tag_seria=0;
            if(substrings.count()>field_index[_TAG_SERIA] && field_index[_TAG_SERIA]>=0)
            {
                tag_seria=substrings[field_index[_TAG_SERIA]].trimmed().toInt();
            }
            QStringList tag_author;
            if(substrings.count()>field_index[_TAG_AUTHOR] && field_index[_TAG_AUTHOR]>=0)
            {
                tag_author=substrings[field_index[_TAG_AUTHOR]].trimmed().split(":");
            }
            qlonglong id_seria=0;
            if(!Seria.isEmpty())
                id_seria=AddSeriaToSQLite(Seria, ExistingLibID_, tag_seria);

            qlonglong t1=QDateTime::currentMSecsSinceEpoch();
            qlonglong id_book;
            if(!bWoDeleted_ || !deleted){
                id_book = AddBookToSQLite(
                    star, name, id_seria, num_in_seria, file, size, id_in_lib, deleted, format, date,
                    language, keys, ExistingLibID_, folder, tag, 0/* //todo нужно реальное значения прочитанности книги*/
                );
                qlonglong t2=QDateTime::currentMSecsSinceEpoch();

                QStringList Authors=substrings[field_index[_AUTHORS]].split(':');
                int author_count=0;
                foreach(QString author,Authors)
                {
                    int tag_auth=0;
                    if(author_count<tag_author.count())
                    {
                        if(!tag_author[author_count].trimmed().isEmpty())
                            tag_auth=tag_author[author_count].trimmed().toInt();
                    }
                    QString sAuthorLow = author.toLower();
                    if(format == "fb2" && sAuthorLow.contains("автор") && (sAuthorLow.contains("неизвестен") || sAuthorLow.contains("неизвестный")))
                    {
                        QString sFile, sArchive;
                        QBuffer buffer;
                        sFile = QString("%1.%2").arg(file).arg(format);
                        sArchive = QString("%1/%2").arg(LibPath_).arg(folder.replace(".inp",".zip"));
                        if(archiveFile.getZipName() != sArchive){
                            archiveFile.close();
                            archiveFile.setZipName(sArchive);
                            if (!archiveFile.open(QuaZip::mdUnzip))
                                qDebug()<<("Error open archive!")<<" "<<sArchive;
                        }
                        QuaZipFile zip_file(&archiveFile);
                        SetCurrentZipFileName(&archiveFile,sFile);
                        if(!zip_file.open(QIODevice::ReadOnly))
                            qDebug()<<"Error open file: "<<sFile;
                        buffer.setData(zip_file.read(16*1024));
                        zip_file.close();

                        QDomDocument doc;
                        doc.setContent(buffer.data());
                        QDomElement title_info=doc.elementsByTagName("title-info").at(0).toElement();
                        QDomNodeList listAuthor=title_info.elementsByTagName("author");

                        for(int i=0;i<listAuthor.count();i++)
                        {
                            QString firstname=listAuthor.at(i).toElement().elementsByTagName("first-name").at(0).toElement().text();
                            QString lastname=listAuthor.at(i).toElement().elementsByTagName("last-name").at(0).toElement().text();
                            QString middlename=listAuthor.at(i).toElement().elementsByTagName("middle-name").at(0).toElement().text();
                            QString nickname = listAuthor.at(i).toElement().elementsByTagName("nickname").at(0).toElement().text();
                            QString sAuthor = author = lastname + "," + firstname + "," + middlename + "," + nickname;
                            sAuthorLow = sAuthor.toLower();
                            if(!sAuthorLow.contains("неизвестен") && !sAuthorLow.contains("неизвестный")){
                                QString sAuthor = QString("%1,%2,%3,%4").arg(lastname, firstname, middlename, nickname);
                                AddAuthorToSQLite(sAuthor,ExistingLibID_,id_book,author_count==0,language,tag_auth);
                                author_count++;
                            }
                        }
                    }else{
                        AddAuthorToSQLite(author,ExistingLibID_,id_book,author_count==0,language,tag_auth);
                        author_count++;
                    }
                }

                qlonglong t3=QDateTime::currentMSecsSinceEpoch();
                if(substrings.count()>=field_index[_GENRES]+1)
                {
                    QStringList Genres=substrings[field_index[_GENRES]].split(':');
                    bool first=true;
                    foreach(QString genre,Genres)
                    {
                        if(!first && genre.trimmed().isEmpty())
                            continue;
                        AddGenreToSQLite(id_book,genre.trimmed(),ExistingLibID_,language);
                        first=false;
                    }
                }
                qlonglong t4=QDateTime::currentMSecsSinceEpoch();
                t1_+=t1-t0;
                t2_+=t2-t1;
                t3_+=t3-t2;
                t4_+=t4-t3;
                count++;
                book_count++;
                if(count==1000)
                {
                    emit Message(tr("Books adds:")+" "+QString::number(book_count));
                    count=0;
                    t1_=0;
                    t2_=0;
                    t3_=0;
                    t4_=0;
                }

            }
        }
        if(count>0)
        {
            emit Message(tr("Books adds: ")+QString::number(book_count));
        }
    }
    dbase.commit();
    emit End();
    dbase.close();
}

void ImportThread::break_import()
{
    loop_ = false;
}

