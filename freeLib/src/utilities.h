#ifndef UTILITIES_H
#define UTILITIES_H

#include <QAbstractButton>
#include <QKeySequence>
#include <QShortcut>
#include <QListWidgetItem>
#include <QPainter>

#include "library.h"

// связывание кнопки с быстрыми клавишами
void BindAnyButtonShortcut(QAbstractButton* button, const QKeySequence& shortcut);
// удаление 'пустого' элемента структуры жанров, который появаляется в результате глюка в Qt по прямому доступу к элементу QMap по его индексу
bool EraseEmptyGenreElement();
// перемещение QListWidgetItem вверх в QListWidget
void MoveQListWidgetItemToUp(QListWidget* lw, const QStringList& list);
// получение данных о файле
QFileInfo GetBookFile(QBuffer& buffer, QBuffer& buffer_info, uint id_book, bool caption = false, QDateTime* file_data = nullptr);
// создание цветной иконки тэга
QPixmap CreateTag(QColor color, int size);
// формирование строки размера файла
QString sizeToString(uint size);

bool SetCurrentZipFileName(QuaZip* zip, const QString& name);

QStringList fillParams(QStringList str, book_info& bi, QFileInfo book_file = QFileInfo());
QString fillParams(QString str, book_info& bi, QFileInfo book_file = QFileInfo());
QStringList fillParams(QStringList str, QFileInfo book_file, QString seria_name, QString book_name, QString author, QString ser_num);
QString fillParams(QString str, QFileInfo book_file, QString seria_name, QString book_name, QString author, QString ser_num);

#endif // UTILITIES_H
