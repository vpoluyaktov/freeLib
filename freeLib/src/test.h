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

    // ������ � ���� ����������� mGenre
    void print_mGenre(bool append = false, int suxffix = 0);
    // ����� � ��������� ������ QMap mGenre �������� ������ ����� �� id ��� ��������� ��������, ���������  � Qt ����: ������ ��������� � mGenre[id].sName ��������� � mGenre ������ �������.
    QString getGroupGenreName(ushort idParrentGenre) const;

private:
    QString testFreeLidDir_;
    QString mGenreFile_;
};

#endif // TEST_H
