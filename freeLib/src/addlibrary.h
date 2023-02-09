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
    // ������������ ������ ��������� � ������� ��� ������� ����������
    void MakeDirsList();
    // ��������� �����������/������������� ���������, � ����������� �� ����� ������ ������� ������ �����
    void SetEnabledOrDisabledControllsOfBooksDirs();
    // ��������� �����������/������������� ���������, � ����������� �� ������� ��������� ������ ������� ������ �����
    void SetEnabledOrDisabledControllsOfSelectedStateItemBooksDirs();
    // ��������� � ������� groups ��������������� �� ��������/������ ������
    void AddGroupToSQLite(qlonglong libID);
    // ��������� ��������� � ��������� ��-���������, ����� ��� �� ����� ����������
    void SetControllsToDefaultState();
    // ���� �� ����������� ���������� � ������ ���������?
    bool IsLibraryWithNameExists(const QString& libraryName, const QString& messageTitle);

private slots:
    void LogMessage(QString msg);
    void InputINPX();
    void SelectBooksDir();
    void StartImport();
    void SelectLibrary();
    void AddNewLibrary();
    // ������ �������� ����������
    void EditLibraryName();
    void DeleteLibrary();
    void EndUpdate();
    void terminateImport();
    void reject();
    void ExistingLibsChanged();
    void ExportLib();
    void ButtonSaveLogClicked();
    // ���������� ������ �������� � ������� � ������ ��������� ����������
    void AddBooksDirToList();
    // �������� ���������� �������� � ������� �� ������ ��������� ����������
    void DeleteDirFromBookDirsList();
    // ���������� ������� ���������/������ ��������� ����� ������ ���� ����������
    void SelectionChangedBookDirsList(const QItemSelection& /*selected*/, const QItemSelection& /*deselected*/);
    // ���������� ������� ��������� ������ � �������� ���� � �������� � �������
    void LineEditBooksDirTextChanged(const QString& text);
    // ��������� ���� ����
    void ExpandLog();

signals:
    void break_import();
};

#endif // ADDLIBRARY_H
