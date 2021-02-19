#ifndef UTILITIES_H
#define UTILITIES_H

#include <QAbstractButton>
#include <QKeySequence>
#include <QShortcut>

// связывание кнопки с быстрыми клавишами
void BindAnyButtonShortcut(QAbstractButton* button, const QKeySequence& shortcut);

#endif // UTILITIES_H
