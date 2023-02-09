#include "common.h"
#include "test.h"

Test::Test(QObject* parent) :
    QObject(parent)
{
    // �������� ����� freeLib ������������� � �������� ����� ������������
    testFreeLidDir_ = QDir::toNativeSeparators(QDir::homePath() + QDir::separator() + "freeLib");
    mGenreFile_ = QDir::toNativeSeparators(testFreeLidDir_ + QDir::separator() + "mGenreFile.txt");
}

Test::~Test()
{
}

/*
    ������ � ���� ����������� mGenre
    ������ �������������: 
    1. � ������������� �����:       Test* test = new Test(this); test->print_mGenre();
    2. � ����������� � ����� �����: Test* test = new Test(this); test->print_mGenre(true);
*/
void Test::print_mGenre(bool append, int suxffix)
{
    QString filename = mGenreFile_;
    QFile file(filename.insert(filename.length()-4, QString::number(suxffix)));
    bool isFileOpen = false;
    if (!append) {
        file.remove();
        isFileOpen = file.open(QIODevice::NewOnly | QIODevice::Text);
    } else isFileOpen = file.open(QIODevice::Append | QIODevice::Text);
    
    if (isFileOpen) {
        QTextStream out(&file);
        out.setCodec("UTF-8");
        out.setGenerateByteOrderMark(false);
        out << "mGenre.size() = " + QString::number(mGenre.size()) << endl;
        QMap <uint,SGenre>::const_iterator ciGenre = mGenre.constBegin();
        QString ParrentGenreName = "";
        while (ciGenre != mGenre.constEnd()) {
            QString GroupGenreName = getGroupGenreName(ciGenre->idParrentGenre);
            if (ParrentGenreName != GroupGenreName) {
                if (ciGenre->idParrentGenre > 0) {
                    out << "id=" + QString::number(ciGenre->idParrentGenre) + ": " + GroupGenreName << endl;
                    out << "    id=" + QString::number(ciGenre->id) + ": " + ciGenre->sName + " (" + ciGenre->sCode + ")" << endl;
                }
            }
            else {
                if (ciGenre->idParrentGenre > 0)
                    out << "    id=" + QString::number(ciGenre->id) + ": " + ciGenre->sName + " (" + ciGenre->sCode + ")" << endl;
            }
            ParrentGenreName = GroupGenreName;
            ++ciGenre;
        }
        out << "mGenre.size() = " + QString::number(mGenre.size()) << endl;
        out << "=============================";
    }
    file.close();
}

/*
    ����� � ��������� ������ QMap mGenre �������� ������ ����� �� id ��� ��������� ��������, ���������  � Qt ����: ������ ��������� � mGenre[id].sName ��������� � mGenre ������ �������.
*/
QString Test::getGroupGenreName(ushort idParrentGenre) const
{
    for (auto it = mGenre.begin(); it != mGenre.end(); it++) {
        if (it.value().idParrentGenre == idParrentGenre)
            return it.value().sName;
    }
    return "";
}
