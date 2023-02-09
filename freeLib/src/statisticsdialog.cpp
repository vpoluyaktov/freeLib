#include <QDebug>
#include <QTextBrowser>
#include <QtSql>
#include <QString>

#include "common.h"

#include "statisticsdialog.h"
#include "ui_statisticsdialog.h"

StatisticsDialog::StatisticsDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::StatisticsDialog)
{
    ui->setupUi(this);
    QString html_start = "<html lang=\"ru\"><head><meta charset = \"utf-8\"></head><body>";
    html_start += "<h1 style=\"text-align:center; font-weight:bold; margin-top:0.25em; margin-bottom:0.25em;\"><font color=\"red\">" + tr("Statistic") + "</font></h1>";
    QString html_end = "</body></html>";
    QString info = html_start +
        GetLibraryInfo(g_idCurrentLib) +
        GetAllDataInfo(g_idCurrentLib) +
        /*GetAuthorsInfo(g_idCurrentLib) +
        GetGenresInfo(g_idCurrentLib) +
        GetSeriasInfo(g_idCurrentLib) +
        GetBooksInfo(g_idCurrentLib) +
        GetGroupsInfo(g_idCurrentLib) +*/
        html_end;
    ui->textBrowserStatistics->append(info);
}

StatisticsDialog::~StatisticsDialog()
{
    delete ui;
}

QString StatisticsDialog::GetLibraryInfo(uint idLibrary)
{
    QString libraryInfo = "";
    QSqlQuery query(QSqlDatabase::database("libdb"));
    query.prepare("SELECT name, path, inpx FROM lib WHERE id=:id_lib;");
    query.bindValue(":id_lib", idLibrary);
    if (!query.exec())
        qDebug() << query.lastError().text();
    if (query.next()) {
        QStringList pathList = query.value(1).toString().trimmed().split("|");
        QString path = "";
        for (int i = 0; i < pathList.count(); i++)
            path += pathList[i] + "<br>";
        path = path.remove(path.length()-4, 4);
        libraryInfo += "<h2><font color=\"blue\">" + tr("Library Info") + ":</font></h2>";
        libraryInfo += "<table border=0>";
        libraryInfo += "<tr><td align=\"right\" style=\"vertical-align: top\"><b><font color=\"navy\">" + tr("Library name") + ": </font></b></td><td>" + query.value(0).toString().trimmed() + "</td></tr>";
        libraryInfo += "<tr><td align=\"right\" style=\"vertical-align: top\"><b><font color=\"navy\">" + tr("Book dirs") + ": </font></b></td><td>" + path + "</td></tr>";
        libraryInfo += "<tr><td align=\"right\" style=\"vertical-align: top\"><b><font color=\"navy\"> inpx " + tr("file") + ": </font></b></td><td>" + query.value(2).toString().trimmed() + "</td></tr>";
        libraryInfo += "</table>";
        libraryInfo += "<hr/>";
    }
    return libraryInfo;
}

QString StatisticsDialog::GetAllDataInfo(uint idLibrary)
{
    QString allDataInfo = "";
    QString authors = "";
    QString genres = "";
    QString serias = "";
    QString books = "";
    QString groups = "";
    QSqlQuery query(QSqlDatabase::database("libdb"));
    query.prepare("SELECT COUNT(id) FROM author WHERE id_lib=:id_lib;");
    query.bindValue(":id_lib", idLibrary);
    if (!query.exec())
        qDebug() << query.lastError().text();
    if (query.next())
        authors = query.value(0).toString().trimmed();

    query.prepare("SELECT COUNT(DISTINCT book_genre.id_genre) FROM book_genre WHERE book_genre.id_lib=:id_lib;");
    query.bindValue(":id_lib", idLibrary);
    if (!query.exec())
        qDebug() << query.lastError().text();
    if (query.next())
        genres = query.value(0).toString().trimmed();

    query.prepare("SELECT COUNT(id) FROM seria WHERE id_lib=:id_lib;");
    query.bindValue(":id_lib", idLibrary);
    if (!query.exec())
        qDebug() << query.lastError().text();
    if (query.next())
        serias = query.value(0).toString().trimmed();

    query.prepare("SELECT COUNT(id) FROM book WHERE id_lib=:id_lib;");
    query.bindValue(":id_lib", idLibrary);
    if (!query.exec())
        qDebug() << query.lastError().text();
    if (query.next())
        books = query.value(0).toString().trimmed();

    query.prepare("SELECT COUNT(id) FROM groups WHERE id_lib=:id_lib;");
    query.bindValue(":id_lib", idLibrary);
    if (!query.exec())
        qDebug() << query.lastError().text();
    if (query.next())
        groups = query.value(0).toString().trimmed();

    allDataInfo += "<h2><font color=\"blue\">" + tr("Library Elements Info") + ":</font></h2>";
    allDataInfo += "<table border=0>";
    allDataInfo += "<tr><td align=\"right\" style=\"vertical-align: top\"><b><font color=\"navy\">" + tr("Author count") + ": </font></b></td><td>" + authors + "</td></tr>";
    allDataInfo += "<tr><td align=\"right\" style=\"vertical-align: top\"><b><font color=\"navy\">" + tr("Genre count") + ": </font></b></td><td>" + genres + "</td></tr>";
    allDataInfo += "<tr><td align=\"right\" style=\"vertical-align: top\"><b><font color=\"navy\">" + tr("Seria count") + ": </font></b></td><td>" + serias + "</td></tr>";
    allDataInfo += "<tr><td align=\"right\" style=\"vertical-align: top\"><b><font color=\"navy\">" + tr("Book count") + ": </font></b></td><td>" + books + "</td></tr>";
    allDataInfo += "<tr><td align=\"right\" style=\"vertical-align: top\"><b><font color=\"navy\">" + tr("Group count") + ": </font></b></td><td>" + groups + "</td></tr>";
    allDataInfo += "</table>";
    allDataInfo += "<hr/>";

    return allDataInfo;
}

QString StatisticsDialog::GetAuthorsInfo(uint idLibrary)
{
    QString authorsInfo = "";
    QSqlQuery query(QSqlDatabase::database("libdb"));
    query.prepare("SELECT COUNT(id) FROM author WHERE id_lib=:id_lib;");
    query.bindValue(":id_lib", idLibrary);
    if (!query.exec())
        qDebug() << query.lastError().text();
    if (query.next()) {
        authorsInfo += "<h2><font color=\"blue\">" + tr("Authors Info") + ":</font></h2>";
        authorsInfo += "<table border=0>";
        authorsInfo += "<tr><td align=\"right\" style=\"vertical-align: top\"><b><font color=\"navy\">" + tr("Author count") + ": </font></b></td><td>" + query.value(0).toString().trimmed() + "</td></tr>";
        authorsInfo += "</table>";
        authorsInfo += "<hr/>";
    }

    return authorsInfo;
}

QString StatisticsDialog::GetGenresInfo(uint idLibrary)
{
    QString genresInfo = "";
    QSqlQuery query(QSqlDatabase::database("libdb"));
    query.prepare("SELECT COUNT(DISTINCT book_genre.id_genre) FROM book_genre WHERE book_genre.id_lib=:id_lib;");
    query.bindValue(":id_lib", idLibrary);
    if (!query.exec())
        qDebug() << query.lastError().text();
    if (query.next()) {
        genresInfo += "<h2><font color=\"blue\">" + tr("Genres Info") + ":</font></h2>";
        genresInfo += "<table border=0>";
        genresInfo += "<tr><td align=\"right\" style=\"vertical-align: top\"><b><font color=\"navy\">" + tr("Genre count") + ": </font></b></td><td>" + query.value(0).toString().trimmed() + "</td></tr>";
        genresInfo += "</table>";
        genresInfo += "<hr/>";
    }

    return genresInfo;
}

QString StatisticsDialog::GetSeriasInfo(uint idLibrary)
{
    QString seriasInfo = "";
    QSqlQuery query(QSqlDatabase::database("libdb"));
    query.prepare("SELECT COUNT(id) FROM seria WHERE id_lib=:id_lib;");
    query.bindValue(":id_lib", idLibrary);
    if (!query.exec())
        qDebug() << query.lastError().text();
    if (query.next()) {
        seriasInfo += "<h2><font color=\"blue\">" + tr("Serias Info") + ":</font></h2>";
        seriasInfo += "<table border=0>";
        seriasInfo += "<tr><td align=\"right\" style=\"vertical-align: top\"><b><font color=\"navy\">" + tr("Seria count") + ": </font></b></td><td>" + query.value(0).toString().trimmed() + "</td></tr>";
        seriasInfo += "</table>";
        seriasInfo += "<hr/>";
    }

    return seriasInfo;
}

QString StatisticsDialog::GetBooksInfo(uint idLibrary)
{
    QString booksInfo = "";
    QSqlQuery query(QSqlDatabase::database("libdb"));
    query.prepare("SELECT COUNT(id) FROM book WHERE id_lib=:id_lib;");
    query.bindValue(":id_lib", idLibrary);
    if (!query.exec())
        qDebug() << query.lastError().text();
    if (query.next()) {
        booksInfo += "<h2><font color=\"blue\">" + tr("Books Info") + ":</font></h2>";
        booksInfo += "<table border=0>";
        booksInfo += "<tr><td align=\"right\" style=\"vertical-align: top\"><b><font color=\"navy\">" + tr("Book count") + ": </font></b></td><td>" + query.value(0).toString().trimmed() + "</td></tr>";
        booksInfo += "</table>";
        booksInfo += "<hr/>";
    }

    return booksInfo;
}

QString StatisticsDialog::GetGroupsInfo(uint idLibrary)
{
    QString groupsInfo = "";
    QSqlQuery query(QSqlDatabase::database("libdb"));
    query.prepare("SELECT COUNT(id) FROM groups WHERE id_lib=:id_lib;");
    query.bindValue(":id_lib", idLibrary);
    if (!query.exec())
        qDebug() << query.lastError().text();
    if (query.next()) {
        groupsInfo += "<h2><font color=\"blue\">" + tr("Groups Info") + ":</font></h2>";
        groupsInfo += "<table border=0>";
        groupsInfo += "<tr><td align=\"right\" style=\"vertical-align: top\"><b><font color=\"navy\">" + tr("Group count") + ": </font></b></td><td>" + query.value(0).toString().trimmed() + "</td></tr>";
        groupsInfo += "</table>";
        groupsInfo += "<hr/>";
    }

    return groupsInfo;
}
