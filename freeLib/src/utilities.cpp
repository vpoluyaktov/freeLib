#include "utilities.h"
#include <quazip/quazip/quazipfile.h>

/*
    связывание кнопки с быстрыми клавишами
*/
void BindAnyButtonShortcut(QAbstractButton* button, const QKeySequence& shortcut)
{
    QObject::connect(new QShortcut(shortcut, button), &QShortcut::activated, [button]() { button->animateClick(); });
}

/*
    удаление 'пустого' элемента структуры жанров, который появаляется в результате глюка в Qt по прямому доступу к элементу QMap по его индексу
*/
bool EraseEmptyGenreElement()
{
    if (mGenre[0].sCode.isEmpty()) {
        for (auto it = mGenre.begin(); it != mGenre.end();)
            if (it.value().sCode.isEmpty()) {
                it = mGenre.erase(it);
                return true;
            }
            else
                ++it;
    }
    return false;
}

/*
    перемещение QListWidgetItem вверх в QListWidget
*/
void MoveQListWidgetItemToUp(QListWidget* lw, const QStringList& list)
{
    for (int i = 0; i <= 2; i++) {
        QListWidgetItem* item = lw->findItems(list[i], Qt::MatchCaseSensitive)[0];
        int row = lw->row(item);
        QListWidgetItem* itemForInsert = lw->takeItem(row);
        lw->insertItem(i, itemForInsert);
    }
}

/*
    получение данных о файле
*/
QFileInfo GetBookFile(QBuffer& buffer_book, QBuffer& buffer_info, uint id_book, bool caption, QDateTime* file_data)
{
    QFileInfo fi;
    SBook& book = mLibs[g_idCurrentLib].mBooks[id_book];
    QString file = book.sFile;
    QString archive;
    if (!book.sArchive.isEmpty()) {
        archive = book.sArchive;
        archive = archive.replace("\\", "/");
    }

    if (archive.isEmpty()) {
        // не zip: только файлы fb2, epub и fbd, расположенный рядом с одноименным не fb2 файлом.
        QFile book_file(file);
        if (!book_file.open(QFile::ReadOnly)) {
            qDebug() << ("Error open file!") << " " << file;
            return fi;
        }
        buffer_book.setData(book_file.readAll());
        fi.setFile(book_file);
        if (file_data)
            *file_data = fi.birthTime();
        fi.setFile(file);
        QString fbd = fi.absolutePath() + "/" + fi.completeBaseName() + ".fbd";
        QFile info_file(fbd);
        if (info_file.exists()) {
            info_file.open(QFile::ReadOnly);
            buffer_info.setData(info_file.readAll());
        }
    }
    else {
        // zip: только fb2.zip и zip файлы, в которых находятся НЕ fb2 книги с fbd файлом рядом с ними (в zip в папке или вне папки).
        QuaZip uz(archive);
        uz.setFileNameCodec(QTextCodec::codecForName("IBM 866"));
        if (!uz.open(QuaZip::mdUnzip)) {
            qDebug() << ("Error open archive!") << " " << archive;
            return fi;
        }

        QString bookPathInZip = "";
        QuaZipFile zip_file(&uz);
        QList<QuaZipFileInfo64> list = uz.getFileInfoList64();
        foreach(QuaZipFileInfo64 str, list) {
            app->processEvents();
            QFileInfo fi(str.name);

            if (fi.suffix() != book.sFormat) // чтение только книги
                continue;

            bookPathInZip = str.name;
            // получение даты создания файла
            if (file_data) {
                SetCurrentZipFileName(&uz, bookPathInZip);
                QuaZipFileInfo64 zip_fi;
                if (uz.getCurrentFileInfo(&zip_fi))
                    *file_data = zip_fi.dateTime;
            }

            // чтение содержимого файла в буффер
            SetCurrentZipFileName(&uz, bookPathInZip);
            if (!zip_file.open(QIODevice::ReadOnly))
                qDebug() << "Error open file: " << file;
            if (caption)
                buffer_book.setData(zip_file.read(16 * 1024));
            else
                buffer_book.setData(zip_file.readAll());
            zip_file.close();

            if (fi.completeBaseName().left(1) != "." && !fi.completeBaseName().isEmpty()) {
                QString fbdPathInZip = fi.path() != "."
                    ? fi.path() + "/" + fi.completeBaseName() + ".fbd"  /* файлы в zip в папке */
                    : fi.completeBaseName() + ".fbd";                   /* файлы в zip без папки */
                // чтение данных описания книги из fbd файла
                if (SetCurrentZipFileName(&uz, book.sFormat == "fb2" ? bookPathInZip : fbdPathInZip)) {
                    zip_file.open(QIODevice::ReadOnly);
                    buffer_info.setData(zip_file.readAll());
                    zip_file.close();
                }
            }
        }

        fi.setFile(archive + "/" + bookPathInZip/*file*/);
    }
    return fi;
}

/*
    создание цветной иконки тэга
*/
QPixmap CreateTag(QColor color, int size)
{
    QPixmap pixmap(size, size - 4);
    pixmap.fill(Qt::transparent);
    QPainter paint(&pixmap);
    paint.setBrush(QBrush(color));
    QPen pen = QPen(QColor(
        static_cast<int>(color.red() * 0.5),
        static_cast<int>(color.green() * 0.5),
        static_cast<int>(color.blue() * 0.5),
        static_cast<int>(color.alpha() * 0.5)
    ));
    paint.setPen(pen);
    paint.drawEllipse(2, 0, size - 5, size - 5);
    return pixmap;
}

/*
    формирование строки размера файла
*/
QString sizeToString(uint size)
{
    QStringList mem;
    mem << QCoreApplication::translate("MainWindow", "B")
        << QCoreApplication::translate("MainWindow", "kB")
        << QCoreApplication::translate("MainWindow", "MB")
        << QCoreApplication::translate("MainWindow", "GB")
        << QCoreApplication::translate("MainWindow", "TB")
        << QCoreApplication::translate("MainWindow", "PB");
    uint rest = 0;
    int mem_i = 0;

    while (size > 1024) {
        mem_i++;
        if (mem_i == mem.count()) {
            mem_i--;
            break;
        }
        rest = size % 1024;
        size = size / 1024;
    }
    double size_d = (float)size + (float)rest / 1024.0;
    return QString("%L1 %2").arg(size_d, 0, 'f', mem_i > 0 ? 1 : 0).arg(mem[mem_i]);
}
