#ifndef ADDLIBRARY_H
#define ADDLIBRARY_H

#include <QDialog>
#include <QItemSelectionModel>
#include <QElapsedTimer>

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

    bool IsLibraryChanged() const;
    
private:
    Ui::AddLibrary *ui;
    ImportThread *imp_tr_;
    QThread *thread_;
    int idCurrentLib_;
    bool bLibChanged_;
    QStringList LogList_;
    QElapsedTimer timer_;

    void ImportNewLibrary(SLib& lib);
    void StartImport(SLib& Lib);
    void SelectLibrary(int idLib);
    void UpdateLibList();

    void SaveLibrary(int idLib, SLib& Lib);
    // формирования списка каталогов с книгами для текущей библиотеки
    void MakeDirsList();
    // установка доступности/недоступности контролов, в зависимости от числа итемов виджета списка папок
    void SetEnabledOrDisabledControllsOfBooksDirs();
    // установка доступности/недоступности контролов, в зависимости от наличия выделения итемов виджета списка папок
    void SetEnabledOrDisabledControllsOfSelectedStateItemBooksDirs();
    // занесение в таблицу groups заблокированные от удаления/правки Группы
    void AddGroupToSQLite(qlonglong libID);
    // установка контролов в состояние по-умолчанию, когда нет ни одной библиотеки
    void SetControllsToDefaultState();
    // Есть ли проверяемая библиотека в списке библиотек?
    bool IsLibraryWithNameExists(const QString& libraryName, const QString& messageTitle);

private slots:
    void LogMessage(QString msg);
    void InputINPX();
    void SelectBooksDir();
    void StartImport();
    void SelectLibrary();
    void AddNewLibrary();
    // правка названия библиотеки
    void EditLibraryName();
    void DeleteLibrary();
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
    // обработчик сигнала выделения/снятия выделения итема списка книг библиотеки
    void SelectionChangedBookDirsList(const QItemSelection& /*selected*/, const QItemSelection& /*deselected*/);
    // обработчик сигнала изменения текста в контроле пути к каталогу с книгами
    void LineEditBooksDirTextChanged(const QString& text);
    // расширить окно лога
    void ExpandLog();

signals:
    void break_import();
};

#endif // ADDLIBRARY_H
