#ifndef ADDLIBRARY_H
#define ADDLIBRARY_H

#include <QDialog>
#include "importthread.h"
#include "common.h"
#include "library.h"

namespace Ui {
class AddLibrary;
}

class AddLibrary : public QDialog
{
    Q_OBJECT

public:
    explicit AddLibrary(QWidget *parent = 0);
    ~AddLibrary();
    //int exec();
    void AddNewLibrary(SLib &lib);
    void StartImport(SLib &Lib);
    void SelectLibrary(int idLib);
    bool IsLibraryChanged() const;
    
private:
    Ui::AddLibrary *ui;
    ImportThread *imp_tr;
    QThread *thread;
    int idCurrentLib_;
    bool bLibChanged;
    QStringList m_LogList;

    void UpdateLibList();
    void SaveLibrary(int idLib, SLib& Lib);
    // формирования списка каталогов с книгами для текущей библиотеки
    void MakeDirsList();
    // установка доступности/недоступности контролов, в зависимости от числа итемов виджета списка папок
    void SetEnabledOrDisabledControllsOfBooksDirs();

private slots:
    void LogMessage(QString msg);
    void InputINPX();
    void SelectBooksDir();
    void StartImport();
    void SelectLibrary();
    void DeleteLibrary();
    void Add_Library();
    void EndUpdate();
    void terminateImport();
    void reject();
    void ExistingLibsChanged();
    void ExportLib();
    void ButtonSaveLogClicked();
    // добавление нового каталога с книгами в список каталогов библиотеки
    void AddBooksDirToList();
    // удаление выбранного каталога с книгами из списка каталогов библиотеки
    void DeleteDirFromBookDirsList();
    // обработчик вставки итема в список папок книг библиотеки
    void InsertItemToBookDirsList();
    // обработчик удаления итема из списка папок книг библиотеки
    void RemoveItemFromBookDirsList();

signals:
    void break_import();
};

#endif // ADDLIBRARY_H
