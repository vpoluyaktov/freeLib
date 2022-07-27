#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <qtextbrowser.h>
#include <QtWidgets/QMainWindow>
#include <QtWidgets/QToolButton>
#include <QSqlQuery>
#include <QtWidgets/QTreeWidgetItem>
#include <QBuffer>
#include <QMenu>
#include <QDropEvent>
#include <QDragEnterEvent>
#include <QTcpServer>
#include <QSystemTrayIcon>

#include "helpdialog.h"
#include "statisticsdialog.h"
#include "dropform.h"
#include "opds_server.h"
#include "common.h"
#include "utilities.h"

namespace Ui {
class MainWindow;
}

struct Stag
{
    QPixmap pm;
    int id;
};

class MainWindow : public QMainWindow
{
    Q_OBJECT
    
public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    bool IsErrorQuit();
    
private:
    Ui::MainWindow *ui;
    opds_server opds_;
    QSystemTrayIcon *trayIcon_;
    DropForm *pDropForm_;
    HelpDialog *pHelpDlg_;
    QToolButton* tbClear_;
    QString lastSearchSymbol_;
    QMenu menuTag_;
    QMenu* menuRating_;
    QMenu* menuReaded_;
    QMenu* menuBookDelete_;
    QObject* currentListForTag_;
    QList<Stag> tagsPicList_;
    QToolButton *FirstButton_;
    QToolButton *langBtnHash_;

    int idCurrentLanguage_;
    bool bUseTag_;
    bool bShowDeleted_;
    bool errorQuit_;
    int IdCurrentGroupGenre_; 
    qlonglong IdSeriaBeforeDeletedSeriaItem_;
    qlonglong IdAuthorBeforeDeletedAuthorItem_;

private:
    QPixmap GetTagFromTagsPicList(int id) const;
    // ���������� ���� ������� ����� ������ ������������
    void UpdateTagsMenu();
    // ���������� ��������� ������ ����� ����� ������ ������������ ��� ������ ���� � ������� ������ ����
    void UpdateBookLanguageControls(int idLibrary);
    // ���������� ��������� ���� �������� ���� �� ������ ������������
    void UpdateExportMenu();
    
    // ���������� �������� ������ ������� �� ���� ��� ��������� ����������
    void FillListWidgetAuthors(int idLibrary);
    // ���������� �������� ������ ����� �� ���� ��� ��������� ����������
    void FillListWidgetSerials(int idLibrary);
    // ���������� �������� ������ ������ �� ���� ��� ��������� ����������
    void FillTreeWidgetGenres(int idLibrary);
    // ���������� �������� ������ ����� �� ���� ��� ��������� ����������
    void FillListWidgetGroups(int idLibrary);
    // ����� (���������) ������, �����, �����, � ����������� �� ��������� ������� ������� �������, ����� ��� ������
    void FillListBooks();
    // ���������� �������� ������ ���� �� ������� � ������ �� ���� ��� ��������� ����������
    void FillListBooks(QList<uint> listBook, uint idCurrentAuthor);
    // ����������� �� �������, ����� ����� ��������� � ������ (���������� ����� � �����, ����������� ��������� ����)
    bool IsMatchingFilterConditions(int idLibrary, const SBook &book) const;
    // ���������� ������ ���� � ������� �������, �����, ����
    void UpdateListPix(qlonglong id, int list, int tag_id);
    void UncheckBooks(QList<qlonglong> list);
    // ���������� '���������' ������� ���������� � id = idLibrary
    void SaveCurrentLibraryState(int idLibrary);
    // ������ �� ���� '���������' ��� ������� ���������� � id = idLibrary
    int LoadCurrentLibraryState(int idLibrary);
    void DeleteDropForm();

    // ����� ���� �� �������� ���������
    QList<uint> StartBooksSearch(
        int idLibrary, const QString& sName, const QString& sAuthor, const QString& sSeria, uint idGenre,
        int idLanguage, int idCurrentTag, const QString& sKeyword, int idCurrentRating,
        bool IsReaded, const QString& sFormat, const QDate& dateFrom, const QDate& dateTo, int nMaxCount
    );
    // ��������� 1-�� �������� ������ �������, ����� ��� �����
    void SelectFirstItemList();
    // ���������� ����� ���������� ���� ������� ���������� � id = idLibrary
    void SaveCurrentBookLanguageFilter(int idLibrary, const QString& lang);
    // ���������� ���������� �������� �� ������� �����
    void FillRatingList();
    // ������� ������ ������� '���������'
    void MarkReadedBook(QTreeWidgetItem* bookItem, bool idReaded);
    // ��������� �����������/������������� ���������, � ����������� �� ����� ������ ������� ������ �����
    void SetEnabledOrDisabledControllsOfSelectedStateItemGroups(const QItemSelection& selected);
    // �������� ���� ���� �� ���������� ������
    void RemoveAllBooksFromGroup(int idLibrary, uint idGroup);
    // ���������� ���������� � ��������� ���� �� ������� �����
    void FillFormatList(int idLibrary);
    // �������� ���� ��������
    void CreateRatingMenu();
    // �������� ���� ���������/�� ���������
    void CreateReadedMenu();
    // �������� ���� �������� �����
    void CreateBookDeleteMenu();
    // �������������� �������� ������ � ������ ����� ���� � ���
    void SetNewGroupNameWithBookCount(int idLibrary, uint idGroup);
    // ����� ���� � ������
    int GetBookCountFromGroup(int idLibrary, uint idGroup);
    // �������� ������ ��� ����� ���� � ���
    QString GetGroupNameWhitoutBookCount(int idLibrary, uint idGroup);
    // �������� ����� ������ �� ���� ������
    void DeleteBookOnlyFromDataBase(uint idBook, QSqlQuery& query);
    // ������ �� ���� '���������' ���� ���������
    void LoadAllLibraries(QSqlQuery& query);
    // ���������� �������� ������� ���������� � id = idLibrary � ��������� ������� ���������
    void FillCurrentLibraryControls(int idLibrary);
    // �������� ����� ������ � �����
    void DeleteBookOnlyFromDisk(uint idBook, QSqlQuery& query);
    // ������ �� ���� ������ ������������ ����� �� �����
    QString ReadBookPathFromLibrary(uint idBook, QSqlQuery& query);
    // ���� ������ ��� ��������� ��������� ������ �� ������, �����, ����� ��� �������� �����
    void CollectDataForBookDelete();

protected:
    APP_MODE mode;

protected:
    void showEvent(QShowEvent *ev);
    void changeEvent(QEvent* event);
    void resizeEvent(QResizeEvent * e);
    void closeEvent(QCloseEvent* event);
    void leaveEvent(QEvent *e);
    void dropEvent(QDropEvent* ev);
    void dragEnterEvent(QDragEnterEvent* ev);
    void dragLeaveEvent(QDragLeaveEvent*);
    void dragMoveEvent(QDragMoveEvent* ev);
    void mouseMoveEvent(QMouseEvent* e);
    void mouseReleaseEvent(QMouseEvent* e);

    // �����������/������������� ������ �������� � �������� ����� �� ������ ������������
    void ExportBookListBtnEnabled(bool Enable);

    void CheckParent(QTreeWidgetItem* parent);
    void CheckChild(QTreeWidgetItem* parent);
    void SendToDevice();
    void SendMail();

    // ���������� ���� ������ ���������
    void FillLibrariesMenu(int idLibrary);
    void FillCheckedBookList(QList<book_info>& list, QTreeWidgetItem* item = nullptr, bool send_all = false, bool count_only = false, bool checked_only = false);
    void FillCheckedItemsBookList(QList<book_info>& list, QTreeWidgetItem* item, bool send_all, bool count_only);

    void ProcPath(QString path, QStringList* book_list);
    void ShowHeaderCoulmn(int nColumn, const QString& sSetting, bool bHide);

private slots:
    // ������ ������-����, ��������
    void ChangingLanguage(bool change_language = true);
    // ������ ������ �����/������ �� ������� ������ �� ������ ������ �������� �����
    void LangBtnSearch();
    // �������� � ����� ������������ ���� ��� ������� �������, ����� � ����
    void ContextMenu(QPoint point);
    // �������� � ����� ������������ ���� ���������� ������� ����
    void HeaderContextMenu(QPoint point);
    // �������� � ����� ������������ ���� ��� ������ �����
    void GroupContextMenu(QPoint point);

    // ���������� ������ ����������� ������ �������
    void btnAuthorClick();
    // ���������� ������ ����������� ������ �����
    void btnSeriesClick();
    // ���������� ������ ����������� ������ ������
    void btnGenresClick();
    // ���������� ������ ����������� ������ ������ ����
    void btnPageSearchClick();
    // ���������� ������ ����� �� ������� �����
    void StartSearch();
    // ���������� ��������� ������ � �������� ������ ������
    void searchChanged(QString str);
    void searchClear();
    // ���������� ������ ����������� ����� ����
    void btnPageGroupsClick();
    // ���������� ������� ���������/������ ��������� ����� ������ �����
    void SelectionChangedGroupsList(const QItemSelection& /*selected*/, const QItemSelection& /*deselected*/);
    // ���������� ������ ���������� ������ � ������ �����
    void AddGroupToList();
    // ���������� ���������� ����� � ������
    void AddBookToGroupAction();
    // ��������� �������� ������
    void RenameGroup();
    // �������� ���������� ����� �� ���������� ������
    void DeleteBookFromGroupAction();
    // �������� ���� ���� �� ���������� ������
    void DeleteAllBooksFromGroup();
    // �������� ������ �� ������ �����
    void RemoveGroupFromList();

    // ������� ���������� ����
    void ExportAction();
    // ��������� �������� (������) �����
    void RatingAction();
    // ��������� �������� ���������/�� ��������� �����
    void ReadedAction();
    // ������ ���������� �����
    void EditBooks();
    // ���������� ������ "��������/����� ������� � ����" 
    void CheckBooks();
    // ���������� ������ "���������" 
    void Settings();
    
    // ��������� ����� �� �� �������� � �������� ����� � �������� � ���� ���������
    void MarkDeletedBooks();
    // ����������� ���� ������
    void DatabaseOptimization();
    // �������� ������ ���������� ���������
    void ShowStatisticsDialog();

    // ����� ���������� ��� �� ��������
    void SelectLibrary();
    // ����� (���������) ������ � ������ �������
    void SelectAuthor();
    // ����� (���������) ����� � ������ �����
    void SelectSeria();
    // ����� (���������) ����� � ������ ������
    void SelectGenre();
    // ����� (���������) ������ � ������ �����
    void SelectGroup();
    // ����� (���������) ����� � ������ ����
    void SelectBook();

    // ���������� ����� ������ �� ������� � �������� �����
    void onAnchorClicked(const QUrl& link);
    // ������� � ���������� ������ � ������ ������� �� ����� �� ������-������ � �������� �����
    void MoveToAuthor(qlonglong id = -1, QString FirstLetter = "");
    // ������� � ��������� ����� � ������ ����� �� ����� �� �����-������ � �������� �����
    void MoveToSeria(qlonglong id = -1, QString FirstLetter = "");
    // ������� � ���������� ����� � ������ ������ �� ����� �� �����-������ � �������� �����
    void MoveToGenre(qlonglong id);

    // ���������� ��������� ������� �����
    void BookItemChanged(QTreeWidgetItem*,int);
    // ���������� �������� ����� �� ��������� �����
    void BookDblClick();
    
    // ���������� ������� ������ �������� ���� � ���������� ������ ������� �����
    void FilterTagSelect(int index);
    // ��������� ������ �������� ���� ��� ������/�����/�����
    void SetTag();

    // ���������� ������������ ���� ����� �� �������� ������ ���������� ������
    void DeleteGroupIconAction();
    // ���������� ������������ ���� ����� �� ���������� ������ ���������� ������
    void SetGroupIconAction();
    // ���������� ������������ ���� ����� �� ������� ������ ��������������� ����� �� ���������
    void SetGroupDefaultIconsAction();
    // ���������� ������������ ���� ����� ���������� �� �����������
    void SortGroupsAscendingOrder();
    // ���������� ������������ ���� ����� ���������� �� ��������
    void SortGroupsDescendingOrder();

    void About();
    void HelpDlg();
    void ShowDropForm();
    void ChangingPort(int i);
    void ChangingTrayIcon(int index = -1, int color = -1);
    void TrayMenuAction(QSystemTrayIcon::ActivationReason reson);
    void dockClicked();
    void MinimizeWindow();
    void actionAboutQt();
    // ���������� ������ "������� ����� ������ �� ���� ������"
    void DeleteBookOnlyFromDataBaseAction();
    // ���������� ������ "������� ����� ������ � �����"
    void DeleteBookOnlyFromDiskAction();
    // ���������� ������ "������� ����� �� ���� ������ � � �����"
    void DeleteBookFromDataBaseAndDiskAction();

    // ���������� ������������ � ����� ���������� �� ����
    void on_actionSwitch_to_convert_mode_triggered();
    // ���������� ������������ � ����� ���������� �� ����
    void on_actionSwitch_to_library_mode_triggered();
    // ���������� ������������ � ����� ���������� �� ���������� �� ������� ������
    void on_btnSwitchToLib_clicked();
    // ���������� ������ ������� �������� �� ���������� �� ������� ������
    void on_btnPreference_clicked();
    // �������� ������� �������, �����, ������, ����, ��������������� ���������� ����� � ���������� ������ ������ �� ������ ������������
    void on_comboBoxLanguageFilter_currentIndexChanged(const QString& arg1);

    //void on_splitter_splitterMoved(int pos, int index);

public slots:
    // ���������� ������ "���������� ������������" 
    void ManageLibrary();

signals:
    void window_loaded();
};

#endif // MAINWINDOW_H
