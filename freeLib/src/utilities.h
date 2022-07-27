#ifndef UTILITIES_H
#define UTILITIES_H

#include <QAbstractButton>
#include <QKeySequence>
#include <QShortcut>
#include <QListWidgetItem>


#include "library.h"

// связывание кнопки с быстрыми клавишами
void BindAnyButtonShortcut(QAbstractButton* button, const QKeySequence& shortcut);
// удаление 'пустого' элемента структуры жанров, который появаляется в результате глюка в Qt по прямому доступу к элементу QMap по его индексу
bool EraseEmptyGenreElement();
// перемещение QListWidgetItem вверх в QListWidget
void MoveQListWidgetItemToUp(QListWidget* lw, const QStringList& list);

#endif // UTILITIES_H
