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

