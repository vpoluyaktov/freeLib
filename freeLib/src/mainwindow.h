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
#include "dropform.h"
#include "opds_server.h"
#include "common.h"

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
    QSystemTrayIcon *trIcon;
    QToolButton *tbClear;
    DropForm *pDropForm;
    HelpDialog *pHelpDlg;
    QString lastSearchSymbol;
    QMenu TagMenu;
    QObject* currentListForTag;
    QList<Stag> tagsPicList;
    opds_server opds;
    QToolButton *FirstButton;
    QToolButton *langBtnHash;

    int idCurrentLanguage_;
    uint idCurrentAuthor_;
    uint idCurrentGenre_;
    uint idCurrentSerial_;
    uint idCurrentBookForAuthor_;
    uint idCurrentBookForGenre_;
    uint idCurrentBookForSeria_;
    bool bUseTag_;
    bool bShowDeleted_;
    QString noSeries_;
    bool errorQuit_;

private:
    QPixmap GetTagFromTagsPicList(int id) const;
    // заполнение меню цветных тегов панели инструментов
    void UpdateTagsMenu();
    // обновление контролов выбора языка книги панели инструментов для списка книг и вкладки поиска книг
    void UpdateBookLanguageControls();
    // обновление контролов меню экспорта книг на панели инструментов
    void UpdateExportMenu();
    
    // заполнение контрола списка Авторов из базы для выбранной библиотеки
    void FillAuthors();
    // заполнение контрола списка Серий из базы для выбранной библиотеки
    void FillSerials();
    // заполнение контрола дерева Жанров из базы для выбранной библиотеки
    void FillGenres();
    // выбор (выделение) Автора, Серии, Жанра, в зависимости от активного виджета списков Авторов, Серий или Жанров
    void FillListBooks();
    // заполнение контрола дерева Книг по Авторам и Сериям из базы для выбранной библиотеки
    void FillListBooks(QList<uint> listBook, uint idCurrentAuthor);

    bool IsBookInList(const SBook &book) const;
    // обновление иконки тэга в списках Авторов, Серий, Книг
    void UpdateListPix(qlonglong id, int list, int tag_id);
    void UncheckBooks(QList<qlonglong> list);
    // сохранение настроек Библиотеки
    void SaveLibPosition();
    void DeleteDropForm();

    // поиск книг по заданным критериям
    QList<uint> StartBooksSearch(
        const QString& sName, const QString& sAuthor, const QString& sSeria, uint idGenre, int idLanguage,
        const QDate& dateFrom, const QDate& dateTo, int nMaxCount
    );
    // Выделение 1-го элемента списка Авторов или Серии
    void SelectFirstItemList();

    // чтение из базы 'позиции' для текущей библиотеки с id = g_idCurrentLib
    int LoadLibraryPosition();

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
    void FillLibrariesMenu();
    //void FillBookList(QSqlQuery& query);
    void FillCheckedBookList(QList<book_info>& list, QTreeWidgetItem* item = nullptr, bool send_all = false, bool count_only = false, bool checked_only = false);
    void FillCheckedItemsBookList(QList<book_info>& list, QTreeWidgetItem* item, bool send_all, bool count_only);

    void ProcPath(QString path, QStringList* book_list);
    void ShowHeaderCoulmn(int nColumn, QString sSetting, bool bHide);

private slots:
    // панель кнопок-букв, символов
    void ChangingLanguage(bool change_language = true);
    // запуск поиска Серии/Автора по нажатию кнопки на панели кнопок символов языка
    void LangBtnSearch();
    // создание и вызов контекстного меню для списков Авторов, Серий и Книг
    void ContextMenu(QPoint point);
    // создание и вызов контекстного меню заголовков таблицы Книг
    void HeaderContextMenu(QPoint point);

    // обработчик кнопки отображения списка Авторов
    void btnAuthor();
    // обработчик кнопки отображения списка Серий
    void btnSeries();
    // обработчик кнопки отображения дерева Жанров
    void btnGenres();
    // обработчик кнопки отображения панели Поиска книг
    void btnPageSearch();
    // обработчик кнопки Найти на вкладке Поиск
    void StartSearch();
    // обработчик изменения текста в контроле строки поиска
    void searchChanged(QString str);
    void searchClear();

    // экспорт выделенных книг
    void ExportAction();
    // установка рейтинга (оценки) книги
    void RatingAction();
    // правка метаданных книги
    void EditBooks();
    // обработчик экшена "Отметить/снять отметки с книг" 
    void CheckBooks();
    // обработчик экшена "Управления библиотеками" 
    void ManageLibrary();
    // обработчик экшена "Настройки" 
    void Settings();

    // Проверить книги на их удаление с жесткого диска и пометить в базе удаленные
    void MarkDeletedBooks();

    // выбор библиотеки для ее загрузки
    void SelectLibrary();
    // выбор (выделение) Автора в списке Авторов
    void SelectAuthor();
    // выбор (выделение) Серии в списке Серий
    void SelectSeria();
    // выбор (выделение) Жанра в дереве Жанров
    void SelectGenre();
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
    
    // обработчик выбора цветного тэга в выпадающем списке цветных тэгов
    void TagSelect(int index);
    // установка иконки цветного тэга для Автора/Серии/Книги
    void SetTag();

    void About();
    void HelpDlg();
    void ShowDropForm();
    void ChangingPort(int i);
    void ChangingTrayIcon(int index = -1, int color = -1);
    void TrayMenuAction(QSystemTrayIcon::ActivationReason reson);
    void dockClicked();
    void MinimizeWindow();

    // обработчик переключения в режим конвертера из меню
    void on_actionSwitch_to_convert_mode_triggered();
    // обработчик переключения в режим библиотеки из меню
    void on_actionSwitch_to_library_mode_triggered();
    // обработчик переключения в режим библиотеки из конвертера по нажатию кнопки
    void on_btnSwitchToLib_clicked();
    // обработчик вызова диалога настроек из конвертера по нажатию кнопки
    void on_btnPreference_clicked();
    // загрузка списков Авторов, Серий, Жанров, книг, соответсвующих выбранному языку в выпадающем списке языков на панели инструментов
    void on_comboBoxLanguageFilter_currentIndexChanged(const QString& arg1);

    //void on_splitter_splitterMoved(int pos, int index);

public slots:
    // обработчик экшена "Помощник добавления библиотеки"
    void newLibWizard(bool AddLibOnly=true);

signals:
    void window_loaded();
};

#endif // MAINWINDOW_H
