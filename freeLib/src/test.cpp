#include "common.h"
#include "test.h"

Test::Test(QObject* parent) :
    QObject(parent)
{
    // тестовая папка freeLib располагается в домашней папке пользователя
    testFreeLidDir_ = QDir::toNativeSeparators(QDir::homePath() + QDir::separator() + "freeLib");
    mGenreFile_ = QDir::toNativeSeparators(testFreeLidDir_ + QDir::separator() + "mGenreFile.txt");
}

Test::~Test()
{
}

/*
    печать в файл содержимого mGenre
    Пример использования: 
    1. С пересозданием файла:       Test* test = new Test(this); test->print_mGenre();
    2. С добавлением в конец файла: Test* test = new Test(this); test->print_mGenre(true);
*/
void Test::print_mGenre(bool append)
{
    QFile file(mGenreFile_);
    bool isFileOpen = false;
    if (!append) {
        file.remove();
        isFileOpen = file.open(QIODevice::NewOnly | QIODevice::Text);
    } else isFileOpen = file.open(QIODevice::Append | QIODevice::Text);
    
    if (isFileOpen) {
        QTextStream out(&file);
        out.setCodec("UTF-8");
        out.setGenerateByteOrderMark(false);
        out << "mGenre.size() = " + QString::number(mGenre.size());
        out << endl;

        auto iGenre = mGenre.constBegin();
        QString ParrentGenreName = "";
        while (iGenre != mGenre.constEnd()) {
            if (ParrentGenreName != mGenre[iGenre->idParrentGenre].sName) {
                out << "id=" + QString::number(iGenre->idParrentGenre) + ": " + mGenre[iGenre->idParrentGenre].sName << endl;;
                if (iGenre->idParrentGenre > 0) {
                    out << "    id=" + QString::number(iGenre.key()) + ": " + QString::number(iGenre->id) << endl;;
                    out << "    id=" + QString::number(iGenre.key()) + ": " + iGenre->sName + "( " + iGenre->sCode + ")" << endl;;
                }
            } else {
                if (iGenre->idParrentGenre > 0) {
                    out << "    id=" + QString::number(iGenre.key()) + ": " + QString::number(iGenre->id) << endl;;
                    out << "    id=" + QString::number(iGenre.key()) + ": " + iGenre->sName + "( " + iGenre->sCode + ")" << endl;;
                }
            }
            ParrentGenreName = mGenre[iGenre->idParrentGenre].sName;
            ++iGenre;
        }

        out << "=============================";
    }
    file.close();
}
