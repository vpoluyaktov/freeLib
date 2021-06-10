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

private:
    QPixmap GetTagFromTagsPicList(int id) const;
    // заполнение меню цветных тегов панели инструментов
    void UpdateTagsMenu();
    // обновление контролов выбора языка книги панели инструментов для списка книг и вкладки поиска книг
    void UpdateBookLanguageControls(int idLibrary);
    // обновление контролов меню экспорта книг на панели инструментов
    void UpdateExportMenu();
    
    // заполнение контрола списка Авторов из базы для выбранной библиотеки
    void FillListWidgetAuthors(int idLibrary);
    // заполнение контрола списка Серий из базы для выбранной библиотеки
    void FillListWidgetSerials(int idLibrary);
    // заполнение контрола дерева Жанров из базы для выбранной библиотеки
    void FillTreeWidgetGenres(int idLibrary);
    // заполнение контрола списка Групп из базы для выбранной библиотеки
    void FillListWidgetGroups(int idLibrary);
    // выбор (выделение) Автора, Серии, Жанра, в зависимости от активного виджета списков Авторов, Серий или Жанров
    void FillListBooks();
    // заполнение контрола дерева Книг по Авторам и Сериям из базы для выбранной библиотеки
    void FillListBooks(QList<uint> listBook, uint idCurrentAuthor);
    // выполняются ли условия, чтобы книга оказалась в списке (фильтрация Языка и Метки, отображения удаленных книг)
    bool IsMatchingFilterConditions(int idLibrary, const SBook &book) const;
    // обновление иконки тэга в списках Авторов, Серий, Книг
    void UpdateListPix(qlonglong id, int list, int tag_id);
    void UncheckBooks(QList<qlonglong> list);
    // сохранение настроек Библиотеки
    void SaveLibPosition(int idLibrary);
    // чтение из базы 'позиции' для текущей библиотеки с id = idLibrary
    int LoadLibraryPosition(int idLibrary);
    void DeleteDropForm();

    // поиск книг по заданным критериям
    QList<uint> StartBooksSearch(
        int idLibrary, const QString& sName, const QString& sAuthor, const QString& sSeria, uint idGenre,
        int idLanguage, int idCurrentTag, const QString& sKeyword, int idCurrentRating,
        bool IsReaded, const QString& sFormat, const QDate& dateFrom, const QDate& dateTo, int nMaxCount
    );
    // выделение 1-го элемента списка Авторов, Жанра или Серии
    void SelectFirstItemList();
    // сохранение языка фильтрации книг текущей библиотеки с id = g_idCurrentLib
    void SaveCurrentBookLanguageFilter(int idLibrary, const QString& lang);
    // заполнение комбобокса рейтинга на вкладке Поиск
    void FillRatingList();
    // пометка ячейки статуса 'Прочитано'
    void MarkReadedBook(QTreeWidgetItem* bookItem, bool idReaded);
    // установка доступности/недоступности контролов, в зависимости от числа итемов виджета списка Групп
    void SetEnabledOrDisabledControllsOfSelectedStateItemGroups(const QItemSelection& selected);
    // удаление всех книг из выделенной группы
    void RemoveAllBooksFromGroup(int idLibrary, uint idGroup);
    // заполнение комбобокса с форматами книг на вкладке Поиск
    void FillFormatList(int idLibrary);
    // создание меню Рейтинга
    void CreateRatingMenu();
    // создание меню Прочитано/Не прочитано
    void CreateReadedMenu();
    // создание меню удаления книги
    void CreateBookDeleteMenu();
    // переименование названия Группы с учетом числа книг в ней
    void SetNewGroupNameWithBookCount(int idLibrary, uint idGroup);
    // число книг в группе
    int GetBookCountFromGroup(int idLibrary, uint idGroup);
    // название Группы без числа книг в ней
    QString GetGroupNameWhitoutBookCount(int idLibrary, uint idGroup);
    // удаление книги только из базы данных
    void DeleteBookOnlyFromDataBase(uint idBook, QSqlQuery& query);
    // обновление структур библиотеки и контролов после удаления книги
    void UpdateLibraryAndControllsAfterBookDelete(uint idBook, QSqlQuery& query);
    // удаление книги только с диска
    void DeleteBookOnlyFromDisk(uint idBook, QSqlQuery& query);
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

    // доступность/недоступность кнопок Экспорта и Открытия книги на панели инструментов
    void ExportBookListBtnEnabled(bool Enable);

    void CheckParent(QTreeWidgetItem* parent);
    void CheckChild(QTreeWidgetItem* parent);
    void SendToDevice();
    void SendMail();

    // Заполнение меню списка Библиотек
    void FillLibrariesMenu(int idLibrary);
    void FillCheckedBookList(QList<book_info>& list, QTreeWidgetItem* item = nullptr, bool send_all = false, bool count_only = false, bool checked_only = false);
    void FillCheckedItemsBookList(QList<book_info>& list, QTreeWidgetItem* item, bool send_all, bool count_only);

    void ProcPath(QString path, QStringList* book_list);
    void ShowHeaderCoulmn(int nColumn, const QString& sSetting, bool bHide);

private slots:
    // панель кнопок-букв, символов
    void ChangingLanguage(bool change_language = true);
    // запуск поиска Серии/Автора по нажатию кнопки на панели кнопок символов языка
    void LangBtnSearch();
    // создание и вызов контекстного меню для списков Авторов, Серий и Книг
    void ContextMenu(QPoint point);
    // создание и вызов контекстного меню заголовков таблицы Книг
    void HeaderContextMenu(QPoint point);
    // создание и вызов контекстного меню для списка Групп
    void GroupContextMenu(QPoint point);

    // обработчик кнопки отображения списка Авторов
    void btnAuthorClick();
    // обработчик кнопки отображения списка Серий
    void btnSeriesClick();
    // обработчик кнопки отображения дерева Жанров
    void btnGenresClick();
    // обработчик кнопки отображения панели Поиска книг
    void btnPageSearchClick();
    // обработчик кнопки Найти на вкладке Поиск
    void StartSearch();
    // обработчик изменения текста в контроле строки поиска
    void searchChanged(QString str);
    void searchClear();
    // обработчик кнопки отображения Групп книг
    void btnPageGroupsClick();
    // обработчик сигнала выделения/снятия выделения итема списка Групп
    void SelectionChangedGroupsList(const QItemSelection& /*selected*/, const QItemSelection& /*deselected*/);
    // обработчик кнопки добавления Группы в список Групп
    void AddGroupToList();
    // добавление выделенной книги в Группу
    void AddBookToGroupAction();
    // изменение названия группы
    void RenameGroup();
    // удаление выделенной книги из выделенной группы
    void DeleteBookFromGroupAction();
    // удаление всех книг из выделенной группы
    void DeleteAllBooksFromGroup();
    // удаление группы из списка групп
    void RemoveGroupFromList();

    // экспорт выделенных книг
    void ExportAction();
    // установка рейтинга (оценки) книги
    void RatingAction();
    // установка признака прочитана/не прочитана книга
    void ReadedAction();
    // правка метаданных книги
    void EditBooks();
    // обработчик экшена "Отметить/снять отметки с книг" 
    void CheckBooks();
    // обработчик экшена "Настройки" 
    void Settings();
    
    // Проверить книги на их удаление с жесткого диска и пометить в базе удаленные
    void MarkDeletedBooks();
    // Оптимизация базы данных
    void DatabaseOptimization();
    // Показать диалог статистики библиотек
    void ShowStatisticsDialog();

    // выбор библиотеки для ее загрузки
    void SelectLibrary();
    // выбор (выделение) Автора в списке Авторов
    void SelectAuthor();
    // выбор (выделение) Серии в списке Серий
    void SelectSeria();
    // выбор (выделение) Жанра в дереве Жанров
    void SelectGenre();
    // выбор (выделение) Группы в списке Групп
    void SelectGroup();
    // выбор (выделение) Книги в списке Книг
    void SelectBook();

    // обработчик клика мышкой на ссылках в описании Книги
    void onAnchorClicked(const QUrl& link);
    // переход к выбранному Автору в списке Авторов по клику на Авторе-ссылке в описании Книги
    void MoveToAuthor(qlonglong id = -1, QString FirstLetter = "");
    // переход к выбранной Серии в списке Серий по клику на Серии-ссылке в описании Книги
    void MoveToSeria(qlonglong id = -1, QString FirstLetter = "");
    // переход к выбранному Жанру в дереве Жанров по клику на Жанре-ссылке в описании Книги
    void MoveToGenre(qlonglong id);

    // обработчик состояния пометки Книги
    void BookItemChanged(QTreeWidgetItem*,int);
    // обработчик двойного клика по выбранной Книге
    void BookDblClick();
    
    // обработчик фильтра выбора цветного тэга в выпадающем списке цветных тэгов
    void FilterTagSelect(int index);
    // установка иконки цветного тэга для Автора/Серии/Книги
    void SetTag();

    // обработчик контекстного меню Групп по удалению иконки выделенной Группы
    void DeleteGroupIconAction();
    // обработчик контекстного меню Групп по присвоению иконки выделенной Группы
    void SetGroupIconAction();
    // обработчик контекстного меню Групп по заданию иконок заблокированных Групп по умолчанию
    void SetGroupDefaultIconsAction();

    void About();
    void HelpDlg();
    void ShowDropForm();
    void ChangingPort(int i);
    void ChangingTrayIcon(int index = -1, int color = -1);
    void TrayMenuAction(QSystemTrayIcon::ActivationReason reson);
    void dockClicked();
    void MinimizeWindow();
    void actionAboutQt();
    // обработчик экшена "Удалить книгу только из базы данных"
    void DeleteBookOnlyFromDataBaseAction();
    // обработчик экшена "Удалить книгу только с диска"
    void DeleteBookOnlyFromDiskAction();
    // обработчик экшена "Удалить книгу из базы данных и с диска"
    void DeleteBookFromDataBaseAndDiskAction();

    // обработчик переключения в режим конвертера из меню
    void on_actionSwitch_to_convert_mode_triggered();
    // обработчик переключения в режим библиотеки из меню
    void on_actionSwitch_to_library_mode_triggered();
    // обработчик переключения в режим библиотеки из конвертера по нажатию кнопки
    void on_btnSwitchToLib_clicked();
    // обработчик вызова диалога настроек из конвертера по нажатию кнопки
    void on_btnPreference_clicked();
    // загрузка списков Авторов, Серий, Жанров, книг, соответствующих выбранному языку в выпадающем списке языков на панели инструментов
    void on_comboBoxLanguageFilter_currentIndexChanged(const QString& arg1);

    //void on_splitter_splitterMoved(int pos, int index);

public slots:
    // обработчик экшена "Управления библиотеками" 
    void ManageLibrary();

signals:
    void window_loaded();
};

#endif // MAINWINDOW_H
