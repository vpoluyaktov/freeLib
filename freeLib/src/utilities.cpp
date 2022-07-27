#include "utilities.h"

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