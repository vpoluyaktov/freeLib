#ifndef TEST_H
#define TEST_H

#include <QMultiMap>
#include <QList>
#include "common.h"
#include "library.h"

class Test : public QObject
{
    Q_OBJECT
public:
    explicit Test(QObject* parent = 0);
    ~Test();

    // печать в файл содержимого mGenre
    void print_mGenre(bool append = false, int suxffix = 0);
    // поиск в структуре данных QMap mGenre Названия Группы жанра по id его дочернего элемента, поскольку  в Qt глюк: прямое обращение к mGenre[id].sName добавляет к mGenre пустой элемент.
    QString getGroupGenreName(ushort idParrentGenre) const;

private:
    QString testFreeLidDir_;
    QString mGenreFile_;
};

#endif // TEST_H
