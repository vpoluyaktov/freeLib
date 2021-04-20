#include <QtSql>
#include <QDomDocument>
#include <QBuffer>
#include <QByteArray>
#include <QSqlDriver>
#include <QSystemTrayIcon>
#include <QCommandLineParser>
#include <QSplashScreen>
#include <QTextCodec>
#include <QMap>
#include <QButtonGroup>
#include <QPainter>
#include <QInputDialog>
#include <QShortcut>

#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "common.h"
#include "quazip/quazip/quazip.h"
#include "quazip/quazip/quazipfile.h"
#include "SmtpClient/smtpclient.h"
#include "SmtpClient/mimefile.h"
#include "SmtpClient/mimetext.h"
#include "SmtpClient/mimeattachment.h"
#include "addlibrary.h"
#include "settingsdlg.h"
#include "exportdlg.h"
#include "exportthread.h"
#include "aboutdialog.h"
#include "tagdialog.h"
#include "bookeditdlg.h"
#include "treebookitem.h"
#include "genresortfilterproxymodel.h"
#include "library.h"
#include <QStandardItemModel>

extern QSplashScreen *splash;

bool db_is_open;

QFileInfo GetBookFile(QBuffer &buffer_book, QBuffer &buffer_info, uint id_book, bool caption, QDateTime *file_data)
{
    QFileInfo fi;
    SBook &book = mLibs[g_idCurrentLib].mBooks[id_book];
    QString file = book.sFile;
    QString archive;
    if (!book.sArchive.isEmpty()) {
        archive = book.sArchive;
        archive = archive.replace("\\", "/");
    }

    if (archive.isEmpty()) {
        // не zip: только файлы fb2, epub и fbd, расположенный рядом с одноименным не fb2 файлом.
        QFile book_file(file);
        if (!book_file.open(QFile::ReadOnly)) {
            qDebug() << ("Error open file!") << " " << file;
            return fi;
        }
        buffer_book.setData(book_file.readAll());
        fi.setFile(book_file);
        if (file_data)
            *file_data = fi.created();
        fi.setFile(file);
        QString fbd = fi.absolutePath() + "/" + fi.completeBaseName() + ".fbd";
        QFile info_file(fbd);
        if (info_file.exists()) {
            info_file.open(QFile::ReadOnly);
            buffer_info.setData(info_file.readAll());
        }
    }
    else {
        // zip: только fb2.zip и zip файлы, в которых находятся НЕ fb2 книги с fbd файлом рядом с ними (в zip в папке или вне папки).
        QuaZip uz(archive);
        uz.setFileNameCodec(QTextCodec::codecForName("IBM 866"));
        if (!uz.open(QuaZip::mdUnzip)) {
            qDebug() << ("Error open archive!") << " " << archive;
            return fi;
        }

        QString bookPathInZip = "";
        QuaZipFile zip_file(&uz);
        QList<QuaZipFileInfo64> list = uz.getFileInfoList64();
        foreach (QuaZipFileInfo64 str, list) {
            app->processEvents();
            QFileInfo fi(str.name);

            if (fi.suffix() != book.sFormat) // чтение только книги
                continue;

            bookPathInZip = str.name;
            // получение даты создания файла
            if (file_data) {
                SetCurrentZipFileName(&uz, bookPathInZip);
                QuaZipFileInfo64 zip_fi;
                if (uz.getCurrentFileInfo(&zip_fi))
                    *file_data = zip_fi.dateTime;
            }

            // чтение содержимого файла в буффер
            SetCurrentZipFileName(&uz, bookPathInZip);
            if (!zip_file.open(QIODevice::ReadOnly))
                qDebug() << "Error open file: " << file;
            if (caption)
                buffer_book.setData(zip_file.read(16 * 1024));
            else
                buffer_book.setData(zip_file.readAll());
            zip_file.close();

            if (fi.completeBaseName().left(1) != "." && !fi.completeBaseName().isEmpty()) {
                QString fbdPathInZip = fi.path() != "."
                    ? fi.path() + "/" + fi.completeBaseName() + ".fbd"  /* файлы в zip в папке */
                    : fi.completeBaseName() + ".fbd";                   /* файлы в zip без папки */
                // чтение данных описания книги из fbd файла
                if (SetCurrentZipFileName(&uz, book.sFormat == "fb2" ? bookPathInZip : fbdPathInZip)) {
                    zip_file.open(QIODevice::ReadOnly);
                    buffer_info.setData(zip_file.readAll());
                    zip_file.close();
                }
            }
        }

        fi.setFile(archive + "/" + bookPathInZip/*file*/);
    }
    return fi;
}

/*
    создание цветной иконки тэга
*/
QPixmap CreateTag(QColor color, int size)
{
    QPixmap pixmap(size, size-4);
    pixmap.fill(Qt::transparent);
    QPainter paint(&pixmap);
    paint.setBrush(QBrush(color));
    QPen pen = QPen(QColor(
        static_cast<int>(color.red()*0.5),
        static_cast<int>(color.green()*0.5),
        static_cast<int>(color.blue()*0.5),
        static_cast<int>(color.alpha()*0.5)
    ));
    paint.setPen(pen);
    paint.drawEllipse(2, 0, size-5, size-5);
    return pixmap;
}

QString sizeToString(uint size)
{
    QStringList mem;
    mem << QCoreApplication::translate("MainWindow", "B")
        << QCoreApplication::translate("MainWindow", "kB")
        << QCoreApplication::translate("MainWindow", "MB")
        << QCoreApplication::translate("MainWindow", "GB")
        << QCoreApplication::translate("MainWindow", "TB")
        << QCoreApplication::translate("MainWindow", "PB");
    uint rest = 0;
    int mem_i = 0;

    while (size > 1024) {
        mem_i++;
        if (mem_i == mem.count()) {
            mem_i--;
            break;
        }
        rest = size % 1024;
        size = size / 1024;
     }
    double size_d = (float)size + (float)rest / 1024.0;
    return QString("%L1 %2").arg(size_d, 0, 'f', mem_i > 0 ? 1 : 0).arg(mem[mem_i]);
}

MainWindow::MainWindow(QWidget* parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    trayIcon_ = nullptr;
    pDropForm_ = nullptr;
    errorQuit_ = false;
    QSettings settings;

    if (db_is_open) {
        QSqlQuery query(QSqlDatabase::database("libdb"));
        //query.exec("CREATE TABLE IF NOT EXISTS params (id INTEGER PRIMARY KEY, name TEXT, value TEXT)");
        query.exec(QString("SELECT value FROM params WHERE name='%1'").arg("version"));
        int version = 0;
        if (query.next()) {
            version = query.value(0).toInt();
        }
        // версия базы данных. Если меньше, то старую удаляем и создаем новую
        if (version < 8) {
            splash->hide();
            if (QMessageBox::question(
                nullptr, tr("Database"),
                tr("This version needs new database version. All your old books data will be lost. Continue?"),
                QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes) {
                if (!openDB(false, true))
                    errorQuit_ = true;
            }
            else {
                errorQuit_ = true;
            }
        }
    }

    ui->setupUi(this);
    ui->btnEdit->setVisible(false);
    ui->lineEditSearchString->setFocus();
    ui->tabWidget->setCurrentIndex(0);
    ui->Books->setColumnWidth(0, 400);
    ui->Books->setColumnWidth(1, 50);
    ui->Books->setColumnWidth(2, 100);
    ui->Books->setColumnWidth(3, 90);
    ui->Books->setColumnWidth(4, 120);
    ui->Books->setColumnWidth(5, 250);
    ui->Books->setColumnWidth(6, 50);
    ui->Books->setColumnWidth(7, 50);
    ui->Books->setColumnWidth(8, 50);

    // деактивировация действий, которые генерируют ссылку в браузере
    ui->Review->setOpenLinks(false);
    ui->Review->setOpenExternalLinks(false);

    connect(
        ui->Review, &QTextBrowser::anchorClicked,
        this, &MainWindow::onAnchorClicked
    );

    setWindowTitle(
        AppName + (g_idCurrentLib < 0 || mLibs[g_idCurrentLib].name.isEmpty()
            ? "" : " - " + mLibs[g_idCurrentLib].name)
    );

    tbClear_ = new QToolButton(this);
    tbClear_->setFocusPolicy(Qt::NoFocus);
    tbClear_->setIcon(QIcon(":/icons/img/icons/clear.png"));
    tbClear_->setStyleSheet("border: none;");
    tbClear_->setCursor(Qt::ArrowCursor);
    tbClear_->setVisible(false);
    QHBoxLayout* layout = new QHBoxLayout(ui->lineEditSearchString);
    layout->addWidget(tbClear_, 0, Qt::AlignRight);
    layout->setSpacing(0);
    layout->setMargin(0);

    idCurrentLanguage_ = -1; // если не был задан язык фильтрации, то это - *, все языки
    bUseTag_ = settings.value("use_tag", true).toBool();
    bShowDeleted_ = settings.value("ShowDeleted").toBool();
    int nCurrentTab;

    if (settings.value("store_position", true).toBool()) {
        // чтение из базы 'позиции' для текущей библиотеки с id = g_idCurrentLib
        nCurrentTab = LoadLibraryPosition(g_idCurrentLib);
    }
    else {
        mLibs[g_idCurrentLib].uIdCurrentAuthor = 0;
        mLibs[g_idCurrentLib].uIdCurrentSeria = 0;
        mLibs[g_idCurrentLib].uIdCurrentGenre = 0;
        mLibs[g_idCurrentLib].uIdCurrentGroup = 0;
        mLibs[g_idCurrentLib].uIdCurrentBookForAuthor = 0;
        mLibs[g_idCurrentLib].uIdCurrentBookForGenre = 0;
        mLibs[g_idCurrentLib].uIdCurrentBookForSeria = 0;
        mLibs[g_idCurrentLib].uIdCurrentBookForGroup = 0;
        mLibs[g_idCurrentLib].uIdCurrentTag = 0;
        nCurrentTab = 0;
    }

    // создание меню Рейтинга
    CreateRatingMenu();
    // создание меню Прочитано/Не прочитано
    CreateReadedMenu();

    UpdateTagsMenu();
    loadGenresFromSQLiteToLibraryStructure();
    loadBooksDataFromSQLiteToLibraryStructure(g_idCurrentLib);
    UpdateBookLanguageControls(g_idCurrentLib);
    // заполнение комбобокса рейтинга на вкладке Поиск
    FillRatingList();
    // заполнение комбобокса с форматами книг на вкладке Поиск
    FillFormatList(g_idCurrentLib);

    FillListWidgetAuthors(g_idCurrentLib);
    FillListWidgetSerials(g_idCurrentLib);
    FillTreeWidgetGenres(g_idCurrentLib);
    FillListWidgetGroups(g_idCurrentLib);

    connect(ui->lineEditSearchString, SIGNAL(/*textEdited*/textChanged(QString)), this, SLOT(searchChanged(QString)));
    connect(tbClear_, SIGNAL(clicked()), this, SLOT(searchClear()));
    connect(ui->actionAddLibrary, SIGNAL(triggered()), this, SLOT(ManageLibrary()));
    connect(ui->btnLibrary, SIGNAL(clicked()), this, SLOT(ManageLibrary()));
    connect(ui->btnOpenBook, SIGNAL(clicked()), this, SLOT(BookDblClick()));
    connect(ui->btnOption, SIGNAL(clicked()), this, SLOT(Settings()));
    connect(ui->actionPreference, SIGNAL(triggered()), this, SLOT(Settings()));
    connect(ui->actionMarkDeletedBooks, SIGNAL(triggered()), this, SLOT(MarkDeletedBooks()));
    connect(ui->actionDatabaseOptimization, &QAction::triggered, this, &MainWindow::DatabaseOptimization);

    connect(ui->actionCheck_uncheck, SIGNAL(triggered()), this, SLOT(CheckBooks()));
    connect(ui->btnCheck, SIGNAL(clicked()), this, SLOT(CheckBooks()));
    connect(ui->btnEdit, SIGNAL(clicked()), this, SLOT(EditBooks()));
    connect(ui->actionExit, SIGNAL(triggered()), this, SLOT(close()));
    #ifdef Q_OS_MACX
        ui->actionExit->setShortcut(QKeySequence(Qt::CTRL|Qt::Key_Q));
    #endif
    #ifdef Q_OS_LINUX
        ui->actionExit->setShortcut(QKeySequence(Qt::CTRL|Qt::Key_Q));
        setWindowIcon(QIcon(":/library.png"));
    #endif
    #ifdef Q_OS_WIN
        ui->actionExit->setShortcut(QKeySequence(Qt::ALT|Qt::Key_F4));
    #endif
    #ifdef Q_OS_WIN32
        ui->actionExit->setShortcut(QKeySequence(Qt::ALT|Qt::Key_F4));
    #endif
    connect(ui->AuthorList, SIGNAL(itemSelectionChanged()), this, SLOT(SelectAuthor()));
    connect(ui->SeriaList, SIGNAL(itemSelectionChanged()), this, SLOT(SelectSeria()));
    connect(ui->GenreList, SIGNAL(itemSelectionChanged()), this, SLOT(SelectGenre()));
    connect(ui->GroupList, SIGNAL(itemSelectionChanged()), this, SLOT(SelectGroup()));
    connect(ui->GroupList->selectionModel(), &QItemSelectionModel::selectionChanged, this, &MainWindow::SelectionChangedGroupsList);
    connect(ui->Books, SIGNAL(itemSelectionChanged()), this, SLOT(SelectBook()));
    connect(ui->Books, SIGNAL(itemDoubleClicked(QTreeWidgetItem*, int)), this, SLOT(BookDblClick()));
    connect(ui->Books, SIGNAL(BookItemChanged(QTreeWidgetItem*, int)), this, SLOT(BookItemChanged(QTreeWidgetItem*, int)));
    connect(ui->btnAuthor, SIGNAL(clicked()), this, SLOT(btnAuthorClick()));
    connect(ui->btnSeries, SIGNAL(clicked()), this, SLOT(btnSeriesClick()));
    connect(ui->btnGenre, SIGNAL(clicked()), this, SLOT(btnGenresClick()));
    connect(ui->btnGroups, SIGNAL(clicked()), this, SLOT(btnPageGroupsClick()));
    connect(ui->btnSearch, SIGNAL(clicked()), this, SLOT(btnPageSearchClick()));
    connect(ui->btnFind, SIGNAL(clicked()), this, SLOT(StartSearch()));
    connect(ui->lineEditFindAuthor, SIGNAL(returnPressed()), this, SLOT(StartSearch()));
    connect(ui->lineEditFindSeria, SIGNAL(returnPressed()), this, SLOT(StartSearch()));
    connect(ui->lineEditFindBookTitle, SIGNAL(returnPressed()), this, SLOT(StartSearch()));
    connect(ui->btnGroupCreate, &QPushButton::clicked, this, &MainWindow::AddGroupToList);
    connect(ui->btnGroupRename, &QPushButton::clicked, this, &MainWindow::RenameGroup);
    connect(ui->btnGroupRemove, &QPushButton::clicked, this, &MainWindow::RemoveGroupFromList);
    connect(ui->btnGroupClear, &QPushButton::clicked, this, &MainWindow::DeleteAllBooksFromGroup);
    connect(ui->actionAbout, SIGNAL(triggered()), this, SLOT(About()));
    connect(ui->btnExpandTreeGenre, &QToolButton::clicked, ui->GenreList, &QTreeWidget::expandAll);
    connect(ui->btnCollapseTreeGenre, &QToolButton::clicked, ui->GenreList, &QTreeWidget::collapseAll);
    connect(ui->actionAboutQtFramework, &QAction::triggered, this, &MainWindow::actionAboutQt);

    // назначение кнопкам быстрых клавиш
    BindAnyButtonShortcut(ui->btnAuthor, QKeySequence("CTRL+A"));
    BindAnyButtonShortcut(ui->btnSeries, QKeySequence("CTRL+S"));
    BindAnyButtonShortcut(ui->btnGenre, QKeySequence("CTRL+G"));
    BindAnyButtonShortcut(ui->btnSearch, QKeySequence("CTRL+F"));
    BindAnyButtonShortcut(ui->btnGroups, QKeySequence("CTRL+P"));

    ChangingLanguage(false);
    // восстановление фильтрации по 1-й букве Автора/Серии (активация символьной кнопки)
    searchChanged(ui->lineEditSearchString->text());
    ExportBookListBtnEnabled(false);

    mode = static_cast<APP_MODE>(settings.value("ApplicationMode", 0).toInt());
    switch (mode)     {
    case MODE_LIBRARY:
        on_actionSwitch_to_library_mode_triggered();
        break;
    case MODE_SHELF:
        //on_actionSwitch_to_shelf_mode_triggered();
        break;
    default:
        connect(this, SIGNAL(window_loaded()), this, SLOT(on_actionSwitch_to_convert_mode_triggered()));
        on_actionSwitch_to_convert_mode_triggered();
        break;
    }


    switch(nCurrentTab) {
    case 0:
        //FillListBooks();
        ui->btnAuthor->click();
        break;
    case 1:
        ui->btnSeries->click();
        break;
    case 2:
        ui->btnGenre->click();
        break;
    case 3:
        ui->btnSearch->click();
        break;
    case 4:
        ui->btnGroups->click();
        break;
    }

    if (ui->lineEditSearchString->text().trimmed().isEmpty())
        FirstButton_->click();

    ui->dateEditFindDateTo->setDate(QDate::currentDate());

    pHelpDlg_ = nullptr;
    connect(ui->actionHelp, SIGNAL(triggered()), this, SLOT(HelpDlg()));
    ui->Books->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->Books, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(ContextMenu(QPoint)));
    ui->AuthorList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->AuthorList, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(ContextMenu(QPoint)));
    ui->SeriaList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->SeriaList, SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(ContextMenu(QPoint)));
    connect(ui->comboBoxTagFilter,SIGNAL(currentIndexChanged(int)), this, SLOT(FilterTagSelect(int)));
    ui->Books->header()->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->Books->header(), SIGNAL(customContextMenuRequested(QPoint)), this, SLOT(HeaderContextMenu(QPoint)));
    ui->GroupList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(ui->GroupList, &QListWidget::customContextMenuRequested, this, &MainWindow::GroupContextMenu);

    opds_.server_run();
    FillLibrariesMenu(g_idCurrentLib);
    UpdateExportMenu();

    setMouseTracking(true);
    centralWidget()->setMouseTracking(true);
    ui->label_drop->setMouseTracking(true);
    ui->frame_drop->setMouseTracking(true);
    ui->pageConvert->setMouseTracking(true);
    ui->stackedWidget->setMouseTracking(true);

    ChangingTrayIcon();

#ifdef Q_OS_OSX
    connect(MyPrivate::instance(), SIGNAL(dockClicked()), SLOT(dockClicked()));
#endif
    connect(ui->actionMinimize_window, SIGNAL(triggered(bool)), SLOT(MinimizeWindow()));

    settings.beginGroup("Columns");
    ui->Books->setColumnHidden(0, !settings.value("ShowName", true).toBool());
    ui->Books->setColumnHidden(1, !settings.value("ShowNumber", true).toBool());
    ui->Books->setColumnHidden(2, !settings.value("ShowSize", true).toBool());
    ui->Books->setColumnHidden(3, !settings.value("ShowMark", true).toBool());
    ui->Books->setColumnHidden(4, !settings.value("ShowImportDate", true).toBool());
    ui->Books->setColumnHidden(5, !settings.value("ShowGenre", true).toBool());
    ui->Books->setColumnHidden(6, !settings.value("ShowLanguage", false).toBool());
    ui->Books->setColumnHidden(7, !settings.value("ShowFormat", true).toBool());
    ui->Books->setColumnHidden(8, !settings.value("ShowReaded", true).toBool());
    QVariant varHeaders = settings.value("headers");
    if (varHeaders.type() == QVariant::ByteArray){
        ui->Books->header()->restoreState(varHeaders.toByteArray());
    }

    // доступность/недоступность кнопок Удалить/Переименовать Группу
    QSqlQuery query(QSqlDatabase::database("libdb"));
    query.prepare("SELECT id FROM groups WHERE id_lib = :id_lib AND blocked = true;");
    query.bindValue(":id_lib", g_idCurrentLib);
    if (!query.exec())
        qDebug() << query.lastError().text();
    while (query.next()) {
        if (mLibs[g_idCurrentLib].uIdCurrentGroup == query.value(0).toInt()) {
            ui->btnGroupRemove->setEnabled(false);
            ui->btnGroupRename->setEnabled(false);
            break;
        }
        else {
            ui->btnGroupRemove->setEnabled(true);
            ui->btnGroupRename->setEnabled(true);
        }
    }

    settings.endGroup();
}

bool MainWindow::IsErrorQuit()
{
    return errorQuit_;
}

void MainWindow::showEvent(QShowEvent *ev)
{
    QMainWindow::showEvent(ev);
    emit window_loaded();
}

QPixmap MainWindow::GetTagFromTagsPicList(int id) const
{
    foreach(Stag tag,tagsPicList_) {
        if (tag.id == id)
            return tag.pm;
    }

    return QPixmap();
}

/*
    заполнение меню цветных тегов панели инструментов
*/
void MainWindow::UpdateTagsMenu()
{
    if (!db_is_open)
        return;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QSettings settings;

    QButtonGroup *group=new QButtonGroup(this);
    group->setExclusive(true);
    const bool wasBlocked = ui->comboBoxTagFilter->blockSignals(true);

    int size = static_cast<int>(ui->comboBoxTagFilter->style()->pixelMetric(QStyle::PM_SmallIconSize) * app->devicePixelRatio());
    QSqlQuery query(QSqlDatabase::database("libdb"));
    // чтение данных тега
    query.exec("SELECT color, name, id FROM tag");
    ui->comboBoxTagFilter->clear();
    ui->comboBoxFindTag->clear();
    int con = 1;
    ui->comboBoxTagFilter->addItem("*", 0);
    ui->comboBoxFindTag->addItem("*",  0);
    menuTag_.clear();
    QAction *ac = new QAction(tr("no tag"), &menuTag_);
    ac->setData(0);
    ac->setShortcut(tr("Ctrl+0"));
    this->addAction(ac); // для срабатывания шортката
    connect(ac, SIGNAL(triggered()), this, SLOT(SetTag()));
    menuTag_.addAction(ac);
    tagsPicList_.clear();
    QPixmap pix = ::CreateTag(QColor(0, 0, 0, 0), size);
    pix.setDevicePixelRatio(app->devicePixelRatio());
    Stag new_tag = {pix, 0};
    tagsPicList_ << new_tag;
    ui->comboBoxTagFilter->setVisible(bUseTag_);
    ui->comboBoxFindTag->setVisible(bUseTag_);
    ui->tag_label->setVisible(bUseTag_);

    while (query.next()) {
        ui->comboBoxTagFilter->addItem(query.value(1).toString().trimmed(), query.value(2).toInt());
        ui->comboBoxFindTag->addItem(query.value(1).toString().trimmed(), query.value(2).toInt());
        if (mLibs[g_idCurrentLib].uIdCurrentTag == ui->comboBoxTagFilter->count() - 1 && bUseTag_)
            ui->comboBoxTagFilter->setCurrentIndex(ui->comboBoxTagFilter->count() - 1);
        pix = ::CreateTag(QColor(query.value(0).toString().trimmed()), size);
        Stag new_tag = {pix, query.value(2).toInt()};
        tagsPicList_ << new_tag;
        ui->comboBoxTagFilter->setItemData(con, pix, Qt::DecorationRole);//Добавляем изображение цвета в комбо
        ui->comboBoxFindTag->setItemData(con, pix, Qt::DecorationRole);  //Добавляем изображение цвета в комбо
        con++;
        QAction *ac = new QAction(pix, query.value(1).toString().trimmed(), &menuTag_);
        ac->setData(query.value(2).toString());
        ac->setShortcut("Ctrl+" + QString::number(query.value(2).toInt()));
        this->addAction(ac); // для срабатывания шортката
        connect(ac, SIGNAL(triggered()), this, SLOT(SetTag()));
        menuTag_.addAction(ac);
    }

    ui->comboBoxTagFilter->addItem(tr("setup ..."), -1);
    ui->comboBoxTagFilter->blockSignals(wasBlocked);

    QApplication::restoreOverrideCursor();
}

MainWindow::~MainWindow()
{
    QSettings settings;
    settings.beginGroup("Columns");
    QByteArray baHeaders = ui->Books->header()->saveState();
    settings.setValue("headers", baHeaders);
    delete ui;
}

/*
    правка метаданных книги
*/
void MainWindow::EditBooks()
{
    BookEditDlg dlg(this);
    dlg.exec();
}

/*
    обновление иконки тэга в списках Авторов, Серий, Книг
*/
void MainWindow::UpdateListPix(qlonglong id, int list,int tag_id)
{
    switch (list) {
    case 1: //авторы
        for (int i = 0; i < ui->AuthorList->count(); i++) {
            if (ui->AuthorList->item(i)->data(Qt::UserRole).toLongLong() == id) {
                ui->AuthorList->item(i)->setIcon(GetTagFromTagsPicList(tag_id));
            }
        }
        break;
    case 2: //серии
        for (int i = 0; i < ui->SeriaList->count(); i++) {
            if (ui->SeriaList->item(i)->data(Qt::UserRole).toLongLong() == id) {
                ui->SeriaList->item(i)->setIcon(GetTagFromTagsPicList(tag_id));
            }
        }

        break;
    }
    for (int i = 0; i < ui->Books->topLevelItemCount(); i++) {
        if (list == 1) {
            if(ui->Books->topLevelItem(i)->data(0, Qt::UserRole).toLongLong()==id)
                ui->Books->topLevelItem(i)->setIcon(0, GetTagFromTagsPicList(tag_id));
        }
        else {
            for (int j = 0; j < ui->Books->topLevelItem(i)->childCount(); j++) {
                if (ui->Books->topLevelItem(i)->child(j)->data(0, Qt::UserRole).toLongLong() == id)
                    ui->Books->topLevelItem(i)->child(j)->setIcon(0, GetTagFromTagsPicList(tag_id));
            }
        }
    }

}

/*
    панель кнопок-букв, символов
*/
void MainWindow::ChangingLanguage(bool change_language)
{
    QSettings settings;
    QFile file(FindLocaleFile(settings.value("localeABC", QLocale::system().name()).toString(), "abc", "txt"));
    QString abc_local = "ABC";
    if (!file.fileName().isEmpty() && file.open(QFile::ReadOnly)) {
        abc_local=QString::fromUtf8(file.readLine()).toUpper();
    }
    QVBoxLayout *layout_abc_all = new QVBoxLayout();

    if (ui->abc->layout()) {

        while (!(qobject_cast<QBoxLayout*>(ui->abc->layout())->itemAt(0))->isEmpty()) {
            delete dynamic_cast<QBoxLayout*>(ui->abc->layout()->itemAt(0))->itemAt(0)->widget();
        }
        if (!qobject_cast<QBoxLayout*>(ui->abc->layout())->isEmpty()) {
            while (!(dynamic_cast<QBoxLayout*>(ui->abc->layout()->itemAt(1)))->isEmpty()) {
                delete dynamic_cast<QBoxLayout*>(ui->abc->layout()->itemAt(1))->itemAt(0)->widget();
            }
        }

        while (!ui->abc->layout()->isEmpty()) {
            delete ui->abc->layout()->itemAt(0);
        }
        delete ui->abc->layout();
    }

    FirstButton_ = nullptr;
    if (abc_local != "ABC") {
        QHBoxLayout *layout_abc = new QHBoxLayout();
        for (int i = 0; i < abc_local.length(); i++) {
            QToolButton *btn = new QToolButton(this);
            btn->setText(abc_local.at(i));
            btn->setMaximumSize(QSize(22, 22));
            btn->setMinimumSize(QSize(22, 22));
            btn->setCheckable(true);
            btn->setAutoExclusive(true);
            btn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
            layout_abc->addWidget(btn);
            connect(btn ,SIGNAL(clicked()), this, SLOT(LangBtnSearch()));
            if (!FirstButton_)
                FirstButton_=btn;
        }
        layout_abc->addStretch();
        layout_abc->setSpacing(1);
        layout_abc->setMargin(0);
        layout_abc_all->addItem(layout_abc);
    }
        QString abc = "*#ABCDEFGHIJKLMNOPQRSTUVWXYZ";
        {
            QHBoxLayout *layout_abc=new QHBoxLayout();
            for (int i = 0;i < abc.length(); i++) {
                QToolButton *btn = new QToolButton(this);
                btn->setText(abc.at(i));
                btn->setMaximumSize(QSize(22, 22));
                btn->setMinimumSize(QSize(22, 22));
                btn->setCheckable(true);
                btn->setAutoExclusive(true);
                btn->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
                layout_abc->addWidget(btn);
                connect(btn, SIGNAL(clicked()), this, SLOT(LangBtnSearch()));
                if (!FirstButton_ && abc.at(i) == 'A')
                    FirstButton_ = btn;
                if (abc.at(i) == '#')
                    langBtnHash_ = btn;
            }
            layout_abc->addStretch();
            layout_abc->setSpacing(1);
            layout_abc->setMargin(0);
#ifdef Q_OS_WIN
            layout_abc->setContentsMargins(0,abc_local != "ABC" ? 4 : 0, 0, 0);
#else
            layout_abc->setContentsMargins(0,abc_local != "ABC" ? 5 : 0, 0, 5);
#endif
            layout_abc_all->addItem(layout_abc);
        }

    ui->abc->setLayout(layout_abc_all);
    ui->abc->layout()->setSpacing(1);
    ui->abc->layout()->setMargin(0);
#ifdef Q_OS_WIN
    ui->abc->layout()->setContentsMargins(0, 4, 0, 5);
#endif
    ui->retranslateUi(this);
    if (change_language)
        FirstButton_->click();
}

/*
    установка иконки цветного тэга для Автора/Серии/Книги
*/
void MainWindow::SetTag()
{
    uchar tag_id=static_cast<uchar>(qobject_cast<QAction*>(QObject::sender())->data().toInt());
    uint id;
    QSqlQuery query(QSqlDatabase::database("libdb"));

    if (currentListForTag_ == qobject_cast<QObject*>(ui->Books)) {
        QTreeWidgetItem* item=ui->Books->selectedItems()[0];
        id=item->data(0,Qt::UserRole).toUInt();
        switch (item->type()) {
        case ITEM_TYPE_BOOK:
            item->setIcon(0, GetTagFromTagsPicList(tag_id));
            query.prepare("UPDATE book set tag=:tag where id=:id");
            query.bindValue(":tag", tag_id);
            query.bindValue(":id", id);
            query.exec();
            mLibs[g_idCurrentLib].mBooks[id].nTag = tag_id;
            break;

        case ITEM_TYPE_SERIA:
            UpdateListPix(id, 2, tag_id);
            query.prepare("UPDATE seria set tag=:tag where id=:id");
            query.bindValue(":tag", tag_id);
            query.bindValue(":id", id);
            query.exec();
            mLibs[g_idCurrentLib].mSerials[id].nTag = tag_id;
            break;

        case ITEM_TYPE_AUTHOR:
            UpdateListPix(id, 1, tag_id);
            query.prepare("UPDATE author set tag=:tag where id=:id");
            query.bindValue(":tag", tag_id);
            query.bindValue(":id", id);
            query.exec();
            mLibs[g_idCurrentLib].mAuthors[id].nTag = tag_id;
            break;

        default:
            break;
        }
    }
    else if(currentListForTag_==qobject_cast<QObject*>(ui->AuthorList)) {
        id=ui->AuthorList->selectedItems()[0]->data(Qt::UserRole).toUInt();
        UpdateListPix(id, 1, tag_id);
        query.prepare("UPDATE author set tag=:tag where id=:id");
        query.bindValue(":tag", tag_id);
        query.bindValue(":id", id);
        query.exec();
        mLibs[g_idCurrentLib].mAuthors[id].nTag = tag_id;
    }
    else if(currentListForTag_ == qobject_cast<QObject*>(ui->SeriaList)) {
        id=ui->SeriaList->selectedItems()[0]->data(Qt::UserRole).toUInt();
        UpdateListPix(id, 2, tag_id);
        query.prepare("UPDATE seria set tag=:tag where id=:id");
        query.bindValue(":tag", tag_id);
        query.bindValue(":id", id);
        query.exec();
        mLibs[g_idCurrentLib].mSerials[id].nTag = tag_id;
    }
}

/*
    обработчик фильтра выбора цветного тэга в выпадающем списке цветных тэгов
*/
void MainWindow::FilterTagSelect(int index)
{
    QSqlQuery query(QSqlDatabase::database("libdb"));
    query.setForwardOnly(true);
    if (ui->comboBoxTagFilter->itemData(ui->comboBoxTagFilter->currentIndex()).toInt() == -1) {
        const bool wasBlocked = ui->comboBoxTagFilter->blockSignals(true);
        // id тега фильтрации текущей библиотеки
        ui->comboBoxTagFilter->setCurrentIndex(mLibs[g_idCurrentLib].uIdCurrentTag);
        ui->comboBoxTagFilter->blockSignals(wasBlocked);
        TagDialog td(this);
        if (td.exec())
            UpdateTagsMenu();
    }
    else if (index >= 0) {
        // сохранение тега фильтрации текущей библиотеки
        query.prepare("UPDATE lib SET currentTag = :currentTag WHERE id = :id_lib;");
        query.bindValue(":currentTag", index);
        query.bindValue(":id_lib", g_idCurrentLib);
        query.exec();

        FillListWidgetAuthors(g_idCurrentLib);
        FillListWidgetSerials(g_idCurrentLib);
        FillTreeWidgetGenres(g_idCurrentLib);
        FillListBooks();
    }
}

/*
    сохранение настроек Библиотеки
*/
void MainWindow::SaveLibPosition(uint idLibrary)
{
    // сохранение в базу данных 'позиции' текущей библиотеки с id = g_idCurrentLib
    QSqlQuery query(QSqlDatabase::database("libdb"));
    query.setForwardOnly(true);
    query.prepare(
        "UPDATE lib SET (currentTab, currentAuthor, currentSeria, currentGenre, currentGroup, currentBookForAuthor, currentBookForSeria, currentBookForGenre, currentBookForGroup, currentSearchingFilter, currentTag, currentBookLanguage) = (:idCurrentTab, :idCurrentAuthor, :idCurrentSeria, :idCurrentGenre, :idCurrentGroup, :idCurrentBookForAuthor, :idCurrentBookForSeria, :idCurrentBookForGenre, :idCurrentBookForGroup, :currentSearchingFilter, :currentTag, :currentBookLanguage) WHERE id = :id_lib;"
    );
    query.bindValue(":idCurrentTab", ui->tabWidget->currentIndex());
    query.bindValue(":idCurrentAuthor", mLibs[idLibrary].uIdCurrentAuthor);
    query.bindValue(":idCurrentSeria", mLibs[idLibrary].uIdCurrentSeria);
    query.bindValue(":idCurrentGenre", mLibs[idLibrary].uIdCurrentGenre);
    query.bindValue(":idCurrentGroup", mLibs[idLibrary].uIdCurrentGroup);
    query.bindValue(":idCurrentBookForAuthor", mLibs[idLibrary].uIdCurrentBookForAuthor);
    query.bindValue(":idCurrentBookForSeria", mLibs[idLibrary].uIdCurrentBookForSeria);
    query.bindValue(":idCurrentBookForGenre", mLibs[idLibrary].uIdCurrentBookForGenre);
    query.bindValue(":idCurrentBookForGroup", mLibs[idLibrary].uIdCurrentBookForGroup);
    query.bindValue(":currentSearchingFilter", ui->lineEditSearchString->text().trimmed());
    query.bindValue(":currentTag", mLibs[idLibrary].uIdCurrentTag);
    query.bindValue(":currentBookLanguage", mLibs[idLibrary].sCurrentBookLanguage);
    query.bindValue(":id_lib", idLibrary);
    query.exec();
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    if (pHelpDlg_ != nullptr)
        delete pHelpDlg_;
    SaveLibPosition(g_idCurrentLib);
    QSettings settings;
    settings.setValue("ApplicationMode", mode);
    if (mode == MODE_LIBRARY) {
        settings.setValue("MainWnd/geometry", saveGeometry());
        settings.setValue("MainWnd/windowState", saveState());
        settings.setValue("MainWnd/tab/geometry", ui->tabWidget->saveGeometry());
        settings.setValue("MainWnd/tab/geometry", ui->splitter->saveState());
        settings.setValue("MainWnd/books/geometry", ui->splitter_2->saveState());
        settings.setValue("MainWnd/books_head/geometry", ui->Books->header()->saveState());
    }
    else {
        settings.setValue("MainWndConvertMode/geometry", saveGeometry());
    }
    if (ui->btnExport->defaultAction())
        settings.setValue("DefaultExport", ui->btnExport->defaultAction()->data().toInt());
    QString TempDir="";
    if (QStandardPaths::standardLocations(QStandardPaths::TempLocation).count()>0)
        TempDir=QStandardPaths::standardLocations(QStandardPaths::TempLocation).at(0);
    QDir(TempDir + "/freeLib/").removeRecursively();
    QMainWindow::closeEvent(event);
}

void MainWindow::ChangingPort(int i)
{
    opds_.server_run(i);
}

/*
    обработчик экшена "Настройки" 
*/
void MainWindow::Settings()
{
    QSettings settings;
    if (ui->btnExport->defaultAction()) {
        settings.setValue("DefaultExport", ui->btnExport->defaultAction()->data().toInt());
    }
    SettingsDlg dlg(this);
    connect(&dlg, SIGNAL(ChangingPort(int)), this, SLOT(ChangingPort(int)));
    connect(&dlg, SIGNAL(ChangingLanguage()), this, SLOT(ChangingLanguage()));
    connect(&dlg, SIGNAL(ChangingTrayIcon(int,int)), this, SLOT(ChangingTrayIcon(int, int)));
    dlg.exec();
    settings.setValue("LibID", g_idCurrentLib);

    bool bShowDeleted = settings.value("ShowDeleted").toBool();
    bool bUseTag = settings.value("use_tag").toBool();
    if (bShowDeleted_ != bShowDeleted || bUseTag_ != bUseTag) {
        bUseTag_ = bUseTag;
        bShowDeleted_ = bShowDeleted;
        UpdateTagsMenu();
//        SaveLibPosition();
        // Проверить книги на их удаление с жесткого диска и пометить в базе удаленные
        MarkDeletedBooks();
    }
    SaveLibPosition(g_idCurrentLib);
    SelectBook();
    opds_.server_run();
    UpdateExportMenu();
    resizeEvent(nullptr);
}

void MainWindow::FillCheckedBookList(QList<book_info> &list, QTreeWidgetItem* item, bool send_all, bool count_only, bool checked_only)
{
    FillCheckedItemsBookList(list, item, send_all, count_only);
    if (list.count() == 0 && !checked_only) {
        if (ui->Books->selectedItems().count() > 0) {
            if (ui->Books->selectedItems()[0]->childCount() > 0)
                FillCheckedItemsBookList(list, ui->Books->selectedItems()[0], true, count_only);
            else {
                if (ui->Books->selectedItems()[0]->parent()) {
                    qlonglong id_book = ui->Books->selectedItems()[0]->data(0, Qt::UserRole).toLongLong();
                    book_info bi;
                    if (!count_only)
                        bi.id = id_book;
                    list << bi;
                }
            }
        }
    }
}

void MainWindow::FillCheckedItemsBookList(QList<book_info> &list, QTreeWidgetItem* item, bool send_all, bool count_only)
{
    QTreeWidgetItem* current;
    for (int i = 0; i < (item?item->childCount():ui->Books->topLevelItemCount()); i++) {
        current=item?item->child(i):ui->Books->topLevelItem(i);
        if (current->childCount() > 0) {
            FillCheckedItemsBookList(list, current, send_all, count_only);
        }
        else {
            if (current->checkState(0) == Qt::Checked || send_all) {
                if (current->parent()) {
                    qlonglong id_book = current->data(0, Qt::UserRole).toLongLong();
                    book_info bi;
                    if (!count_only)
                        bi.id = id_book;
                    list << bi;
                }
            }
        }
    }
}


void MainWindow::UncheckBooks(QList<qlonglong> list)
{
    QSettings settings;
    if (!settings.value("uncheck_export", true).toBool()) {
        return;
    }
    QList<QTreeWidgetItem*> items;
    if (ui->Books->topLevelItemCount() == 0) {
        return;
    }
    foreach (qlonglong id, list) {
        for (int i = 0; i < ui->Books->topLevelItemCount(); i++) {
            items<<ui->Books->topLevelItem(i);
        }
        do {
            if (items[0]->childCount() > 0) {
                for (int j = 0; j < items[0]->childCount(); j++) {
                    items<<items[0]->child(j);
                }
            }
            else {
                if (items[0]->data(0, Qt::UserRole).toLongLong() == id && items[0]->checkState(0) == Qt::Checked) {
                    items[0]->setCheckState(0, Qt::Unchecked);
                }
            }
            items.removeAt(0);
        } while(items.count() > 0);
        items.clear();
    }
}

void MainWindow::SendToDevice()
{
    QList<book_info> book_list;
    FillCheckedBookList(book_list);
    if (book_list.count() == 0)
        return;
    ExportDlg dlg(this);
    dlg.exec(
        book_list, ST_Device, (ui->btnAuthor->isChecked()
        ? ui->AuthorList->selectedItems()[0]->data(Qt::UserRole).toLongLong() : 0)
    );
    UncheckBooks(dlg.succesfull_export_books);
}

void MainWindow::SendMail()
{
    QList<book_info> book_list;
    FillCheckedBookList(book_list);
    if (book_list.count() == 0)
        return;
    ExportDlg dlg(this);
    dlg.exec(
        book_list,ST_Mail, (ui->btnAuthor->isChecked()
            ? ui->AuthorList->selectedItems()[0]->data(Qt::UserRole).toLongLong() : 0)
    );
    UncheckBooks(dlg.succesfull_export_books);
}

/*
    обработчик двойного клика по выбранной Книге
*/
void MainWindow::BookDblClick()
{
    if (ui->Books->selectedItems().count() == 0)
        return;
    QTreeWidgetItem* item = ui->Books->selectedItems()[0];
    QBuffer buffer_book;
    QBuffer buffer_info;
    QFileInfo fi = GetBookFile(buffer_book, buffer_info, item->data(0, Qt::UserRole).toUInt());
    if (fi.fileName().isEmpty())
        return;
    QString TempDir = "";
    if (QStandardPaths::standardLocations(QStandardPaths::TempLocation).count() > 0)
        TempDir = QStandardPaths::standardLocations(QStandardPaths::TempLocation).at(0);
    QDir dir(TempDir + "/freeLib");
    dir.mkpath(dir.path());
    QFile file(dir.path() + "/" + fi.fileName());
    file.open(QFile::WriteOnly);
    file.write(buffer_book.data());
    file.close();

    QSettings settings;
    int count=settings.beginReadArray("application");
    // проверить цикл
    for (int i=0; i < count; i++) {
        settings.setArrayIndex(i);
        if ((settings.value("ext").toString() + ";").toLower().contains(fi.suffix().toLower() + ";")) {
            if(
#ifdef Q_OS_MACX
            QProcess::startDetached("open", QStringList()<<settings.value("app").toString() << "--args" << file.fileName()) &&
                    QFileInfo(settings.value("app").toString()).exists()
#else
            QProcess::startDetached(settings.value("app").toString(),QStringList() << file.fileName())
#endif
            )
                settings.endArray();
                return;
        }
    }
    settings.endArray();

    QDesktopServices::openUrl(QUrl::fromLocalFile(file.fileName()));
    settings.sync();
}

/*
    Проверить книги на их удаление с жесткого диска и пометить в базе удаленные
*/
void MainWindow::MarkDeletedBooks()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    qint64 t_start = QDateTime::currentMSecsSinceEpoch();

    QSqlQuery query(QSqlDatabase::database("libdb"));
    query.setForwardOnly(true);
    
    /*QString LibPath = mLibs[g_idCurrentLib].path;
    LibPath = RelativeToAbsolutePath(LibPath);*/

    QHash<uint, SBook>::const_iterator iBook = mLibs[g_idCurrentLib].mBooks.constBegin();
    while (iBook != mLibs[g_idCurrentLib].mBooks.constEnd()) {
        uint BookId = iBook.key();
        SBook& book = mLibs[g_idCurrentLib].mBooks[BookId];
        // проверка, есть ли эта книга на жестком диске. Если нет, то в базу Deleted = true
        QString BookPath;
        if (book.sArchive.isEmpty())
            BookPath = book.sFile; // QString("%1%2%3.%4").arg(LibPath, QDir::separator(), book.sFile, book.sFormat);
        else
            BookPath = book.sArchive; // QString("%1%2%3").arg(LibPath, QDir::separator(), book.sArchive);
        QFile file;
        if (file.exists(BookPath))
            book.bDeleted = false;
        else
            book.bDeleted = true;

        query.prepare("UPDATE book set deleted=:deleted where id=:id");
        query.bindValue(":deleted", book.bDeleted);
        query.bindValue(":id", BookId);
        if (!query.exec())
            qDebug() << query.lastError().text();

        ++iBook;
    }

    // перезагрузка книг для изменения цвета итемов удаленных книг
    FillListWidgetAuthors(g_idCurrentLib);
    FillListWidgetSerials(g_idCurrentLib);
    FillTreeWidgetGenres(g_idCurrentLib);
    FillListWidgetGroups(g_idCurrentLib);
    FillListBooks();

    qint64 t_end = QDateTime::currentMSecsSinceEpoch();
    qDebug() << "MarkDeletedBooks " << t_end - t_start << "msec";
    QApplication::restoreOverrideCursor();
    QMessageBox::information(this, tr("Mark deleted books in base"), tr("The search for deleted books in the storage and marking them in the database is completed."));
}

/*
    обработчик экшена "Отметить/снять отметки с книг" 
*/
void MainWindow::CheckBooks()
{
    QList<book_info> book_list;
    FillCheckedBookList(book_list, nullptr, false, true, true);

    const QSignalBlocker blocker( ui->Books);
    Qt::CheckState cs = book_list.count() > 0 ? Qt::Unchecked : Qt::Checked;
    for (int i = 0; i < ui->Books->topLevelItemCount(); i++) {
        ui->Books->topLevelItem(i)->setCheckState(0, cs);
        CheckChild(ui->Books->topLevelItem(i));
    }
}

void MainWindow::CheckParent(QTreeWidgetItem *parent)
{
    bool checked = false;
    bool unchecked = false;
    bool partially = false;
    for (int i = 0; i < parent->childCount(); i++) {
        switch (parent->child(i)->checkState(0)) {
        case Qt::Checked:
            checked = true;
            break;
        case Qt::Unchecked:
            unchecked = true;
            break;
        case Qt::PartiallyChecked:
            partially = true;
            break;
        }
    }
    if (partially || (checked && unchecked))
        parent->setCheckState(0, Qt::PartiallyChecked);
    else if(checked)
        parent->setCheckState(0, Qt::Checked);
    else
        parent->setCheckState(0, Qt::Unchecked);
    if (parent->parent())
        CheckParent(parent->parent());

}
void MainWindow::CheckChild(QTreeWidgetItem *parent)
{
    if (parent->childCount() > 0) {
        for (int i = 0; i < parent->childCount(); i++) {
            parent->child(i)->setCheckState(0, parent->checkState(0));
            if (parent->child(i)->childCount() > 0)
                CheckChild(parent->child(i));
        }
    }
}

/*
    обработчик состояния пометки Книги
*/
void MainWindow::BookItemChanged(QTreeWidgetItem *item, int)
{
    const bool wasBlocked = ui->Books->blockSignals(true);
    CheckChild(item);
    QTreeWidgetItem* parent=item->parent();
    if (parent)
        CheckParent(parent);
    QList<book_info> book_list;
    FillCheckedBookList(book_list, nullptr, false, true);
    ExportBookListBtnEnabled(book_list.count() != 0);

    ui->Books->blockSignals(wasBlocked);
}

/*
    доступность/недоступность кнопок Экспорта и Открытия книги на панели инструментов
*/
void MainWindow::ExportBookListBtnEnabled(bool Enable)
{
    ui->btnExport->setEnabled(Enable);
    ui->btnOpenBook->setEnabled(false);
}

/*
    обработчик кнопки Найти на вкладке Поиск
*/
void MainWindow::StartSearch()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    ui->Books->clear();
    ui->labelFindBooks->setText("0");
    ui->comboBoxLanguageFilter->setCurrentIndex(0);
    ui->comboBoxTagFilter->setCurrentIndex(0);
    app->processEvents();
    ExportBookListBtnEnabled(false);
    QString sName = ui->lineEditFindBookTitle->text().trimmed();
    QString sAuthor = ui->lineEditFindAuthor->text().trimmed();
    QString sSeria = ui->lineEditFindSeria->text().trimmed();
    QDate dateFrom = ui->dateEditFindDateFrom->date();
    QDate dateTo = ui->dateEditFindDateTo->date();
    int nMaxCount = ui->spinBoxFindMaxBooks->value();
    uint idGenre = ui->comboBoxFindGenre->currentData().toUInt();
    int idLanguage = ui->comboBoxFindLanguage->currentData().toInt();
    int idCurrentTag = ui->comboBoxFindTag->itemData(ui->comboBoxFindTag->currentIndex()).toInt();
    int idCurrentRating = ui->comboBoxFindRating->currentData().toInt();
    QString sKeyword = ui->lineEditFindKeywords->text().trimmed();
    bool IsReaded = ui->checkBoxFindReaded->isChecked();
    QString sFormat = ui->comboBoxFindFormat->currentText();

    // Поиск книг по заданным критериям
    QList<uint> listBooks;
    if (idGenre == 0) // * - книги всех Жанров
        listBooks = StartBooksSearch(
            g_idCurrentLib, sName, sAuthor, sSeria, idGenre, idLanguage, idCurrentTag,
            sKeyword, idCurrentRating, IsReaded, sFormat, dateFrom, dateTo, nMaxCount
        );
    else {
        // проверяем, Группа ли это Жанров или Жанр
        // читаем из базы id_parent для выбранного элемента контролов Жанров
        QSqlQuery query(QSqlDatabase::database("libdb"));
        query.setForwardOnly(true);
        query.prepare("SELECT id_parent FROM genre WHERE id=:id;");
        query.bindValue(":id", idGenre);
        if (!query.exec())
            qDebug() << query.lastError().text();
        query.next();
        uint idParrentGenre = query.value(0).toUInt();

        if (idParrentGenre > 0) // Жанр
            listBooks = StartBooksSearch(
                g_idCurrentLib, sName, sAuthor, sSeria, idGenre, idLanguage, idCurrentTag,
                sKeyword, idCurrentRating, IsReaded, sFormat, dateFrom, dateTo, nMaxCount
            );
        else {
            // Группа Жанров: собираем в список id всех Жанров этой Группы
            QList<uint> GenreList;
            QList<QTreeWidgetItem*> ItemList = ui->GenreList->findItems(ui->comboBoxFindGenre->currentText().trimmed(), Qt::MatchFixedString);
            int childCount = ItemList[0]->childCount();
            for (int i = 0; i < childCount; i++) {
                QTreeWidgetItem* childItem = ItemList[0]->child(i);
                GenreList << childItem->data(0, Qt::UserRole).toUInt();
            }
            QList<uint> listBooksForCurrentGenre;
            foreach(uint uGenreId, GenreList) {
                listBooksForCurrentGenre.clear();
                listBooksForCurrentGenre << StartBooksSearch(
                    g_idCurrentLib, sName, sAuthor, sSeria, uGenreId, idLanguage, idCurrentTag,
                    sKeyword, idCurrentRating, IsReaded, sFormat, dateFrom, dateTo, nMaxCount
                );
                // защита от добавления одной и той же книги, но другого Жанра этой же Группы
                foreach(uint id, listBooksForCurrentGenre) {
                    if (!listBooks.contains(id))
                        listBooks << id;
                }
            }
        }
    }

    ui->labelFindBooks->setText(QString::number(listBooks.count()));
    FillListBooks(listBooks, 0);

    QApplication::restoreOverrideCursor();
}

/*
    Поиск книг по заданным критериям
*/
QList<uint> MainWindow::StartBooksSearch(
    uint idLibrary, const QString& sName, const QString& sAuthor, const QString& sSeria, uint idGenre,
    int idLanguage, int idCurrentTag, const QString& sKeyword, int idCurrentRating,
    bool IsReaded, const QString& sFormat, const QDate& dateFrom, const QDate& dateTo, int nMaxCount
)
{
    QList<uint> listBooks;
    int nCount = 0;
    auto iBook = mLibs[idLibrary].mBooks.constBegin();
    while (iBook != mLibs[idLibrary].mBooks.constEnd()) {
        if ((bShowDeleted_ || !iBook->bDeleted) &&
            iBook->date >= dateFrom && iBook->date <= dateTo &&
            (sAuthor.isEmpty() || mLibs[idLibrary].mAuthors[iBook->idFirstAuthor].getName().contains(sAuthor, Qt::CaseInsensitive)) &&
            (sName.isEmpty() || iBook->sName.contains(sName, Qt::CaseInsensitive)) &&
            (sSeria.isEmpty() || (iBook->idSerial > 0 && mLibs[idLibrary].mSerials[iBook->idSerial].sName.contains(sSeria, Qt::CaseInsensitive))) &&
            (idLanguage == -1 || (iBook->idLanguage == idLanguage)) &&
            (!bUseTag_ || idCurrentTag == 0 || idCurrentTag == iBook->nTag
                || (iBook->idSerial > 0 && mLibs[idLibrary].mSerials[iBook->idSerial].nTag == idCurrentTag)
                || (mLibs[idLibrary].mAuthors[iBook->idFirstAuthor].nTag == idCurrentTag)) &&
            (idCurrentRating == -1 || (iBook->nStars == idCurrentRating)) &&
            (sKeyword.isEmpty() || iBook->sKeywords.contains(sKeyword, Qt::CaseInsensitive)) &&
            (IsReaded ? iBook->bReaded : true) &&
            (sFormat != "*" ? (sFormat == iBook->sFormat) : true))
        {
            if (idGenre == 0) {
                nCount++;
                listBooks << iBook.key();
            } else {
                foreach(uint id, iBook->listIdGenres) {
                    if (id == idGenre) {
                        nCount++;
                        if (!listBooks.contains(iBook.key()))
                            listBooks << iBook.key();
                        break;
                    }
                }
            }
        }
        ++iBook;
        if (nCount == nMaxCount)
            break;
    }
    return listBooks;
}

/*
    выбор библиотеки для ее загрузки
*/
void MainWindow::SelectLibrary()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    SaveLibPosition(g_idCurrentLib);
    ui->Books->clear();
    idCurrentLanguage_ = -1; // если не был задан язык фильтрации, то это - *, все языки
    QAction* action=qobject_cast<QAction*>(sender());
    QSettings settings;
    settings.setValue("LibID",action->data().toLongLong());
    g_idCurrentLib = action->data().toInt();
    int nCurrentTab;
    if (settings.value("store_position", true).toBool()) {
        // чтение из базы 'позиции' для текущей библиотеки с id = g_idCurrentLib
        nCurrentTab = LoadLibraryPosition(g_idCurrentLib);
    }

    loadBooksDataFromSQLiteToLibraryStructure(g_idCurrentLib);
    UpdateTagsMenu();
    UpdateBookLanguageControls(g_idCurrentLib);
    // заполнение комбобокса с форматами книг на вкладке Поиск
    FillFormatList(g_idCurrentLib);

    FillListWidgetAuthors(g_idCurrentLib);
    FillListWidgetSerials(g_idCurrentLib);
    FillTreeWidgetGenres(g_idCurrentLib);
    FillListWidgetGroups(g_idCurrentLib);

    searchChanged(ui->lineEditSearchString->text());
    setWindowTitle(
        AppName + (g_idCurrentLib < 0 || mLibs[g_idCurrentLib].name.isEmpty() ? "" : " - " + mLibs[g_idCurrentLib].name)
    );
    FillLibrariesMenu(g_idCurrentLib);

    if (settings.value("store_position", true).toBool()) {
        switch (nCurrentTab)
        {
        case 0:
            ui->btnAuthor->click();
            break;
        case 1:
            ui->btnSeries->click();
            break;
        case 2:
            ui->btnGenre->click();
            break;
        case 3:
            ui->btnSearch->click();
            break;
        case 4:
            ui->btnGroups->click();
            break;
        }
    }
    else {
        SelectFirstItemList(); // Выделение 1-го элемента списка Авторов или Серии
        FillListBooks();
    }

    QApplication::restoreOverrideCursor();
}

/*
    выбор (выделение) Автора в списке Авторов
*/
void MainWindow::SelectAuthor()
{
    ExportBookListBtnEnabled(false);
    if (ui->AuthorList->selectedItems().count() == 0)
        return;

    currentListForTag_ = qobject_cast<QObject*>(ui->AuthorList);

    QListWidgetItem* selectedItem = ui->AuthorList->selectedItems()[0];
    mLibs[g_idCurrentLib].uIdCurrentAuthor = selectedItem->data(Qt::UserRole).toUInt();
    QSettings settings;
    if (settings.value("store_position", true).toBool()) {
        QSqlQuery query(QSqlDatabase::database("libdb"));
        query.setForwardOnly(true);
        query.prepare("UPDATE lib SET currentAuthor = :currentAuthor WHERE id=:id_lib");
        query.bindValue(":currentAuthor", mLibs[g_idCurrentLib].uIdCurrentAuthor);
        query.bindValue(":id_lib", g_idCurrentLib);
        query.exec();
    }

    // Выделение жирным выбранного Автора
    QListWidgetItem* item = nullptr;
    QFont font = ui->AuthorList->font();
    for (int i = 0; i < ui->AuthorList->count(); ++i) {
        item = ui->AuthorList->item(i);
        if (item != selectedItem)
            font.setBold(false);
        else
            font.setBold(true);
        item->setFont(font);
    }

    // скроллинг до выделенного Автора
    ui->AuthorList->scrollToItem(selectedItem);

    // заполнение контрола дерева Книг по Авторам и Сериям из базы для выбранной библиотеки
    QList<uint> listBooks = mLibs[g_idCurrentLib].mAuthorBooksLink.values(mLibs[g_idCurrentLib].uIdCurrentAuthor);
    FillListBooks(listBooks, mLibs[g_idCurrentLib].uIdCurrentAuthor);
}

/*
    выбор (выделение) Серии в списке Серий
*/
void MainWindow::SelectSeria()
{
    ui->Books->clear();
    ExportBookListBtnEnabled(false);
    if (ui->SeriaList->selectedItems().count() == 0)
        return;

    currentListForTag_ = qobject_cast<QObject*>(ui->SeriaList);

    QListWidgetItem* selectedItem = ui->SeriaList->selectedItems()[0];
    mLibs[g_idCurrentLib].uIdCurrentSeria = selectedItem->data(Qt::UserRole).toUInt();

    QSettings settings;
    if (settings.value("store_position", true).toBool()) {
        QSqlQuery query(QSqlDatabase::database("libdb"));
        query.setForwardOnly(true);
        query.prepare("UPDATE lib SET currentSeria = :currentSeria WHERE id = :id_lib");
        query.bindValue(":currentSeria", mLibs[g_idCurrentLib].uIdCurrentSeria);
        query.bindValue(":id_lib", g_idCurrentLib);
        query.exec();
    }

    QList<uint> listBooks;
    QHash<uint, SBook>::const_iterator iBook = mLibs[g_idCurrentLib].mBooks.constBegin();
    while (iBook != mLibs[g_idCurrentLib].mBooks.constEnd()) {
        if (iBook->idSerial == mLibs[g_idCurrentLib].uIdCurrentSeria &&
            (idCurrentLanguage_ == -1 || idCurrentLanguage_ == iBook->idLanguage))
            listBooks << iBook.key();
        ++iBook;
    }

    // Выделение жирным выбранной Серии
    QListWidgetItem* item = nullptr;
    QFont font = ui->SeriaList->font();
    for (int i = 0; i < ui->SeriaList->count(); ++i) {
        item = ui->SeriaList->item(i);
        if (item != selectedItem)
            font.setBold(false);
        else
            font.setBold(true);
        item->setFont(font);
    }

    // скроллинг до выделенной Серии
    ui->SeriaList->scrollToItem(selectedItem);

    // заполнение контрола дерева Книг по Авторам и Сериям из базы для выбранной библиотеки
    FillListBooks(listBooks, 0);
}

/*
    выбор (выделение) Жанра в дереве Жанров
*/
void MainWindow::SelectGenre()
{
    ui->Books->clear();
    ExportBookListBtnEnabled(false);
    if (ui->GenreList->selectedItems().count() == 0)
        return;
    
    currentListForTag_ = qobject_cast<QObject*>(ui->GenreList);
    
    QTreeWidgetItem* selectedItem = ui->GenreList->selectedItems()[0];
    mLibs[g_idCurrentLib].uIdCurrentGenre = selectedItem->data(0, Qt::UserRole).toUInt();
    QList<uint> listBooks;
    QHash<uint, SBook>::const_iterator iBook = mLibs[g_idCurrentLib].mBooks.constBegin();
    while (iBook != mLibs[g_idCurrentLib].mBooks.constEnd()) {
        if ((idCurrentLanguage_ == -1 || idCurrentLanguage_ == iBook->idLanguage)) {
            foreach(uint iGenre, iBook->listIdGenres) {
                if (iGenre == mLibs[g_idCurrentLib].uIdCurrentGenre) {
                    listBooks << iBook.key();
                    break;
                }
            }
        }
        ++iBook;
    }

    QSettings settings;
    if (settings.value("store_position", true).toBool()) {
        QSqlQuery query(QSqlDatabase::database("libdb"));
        query.setForwardOnly(true);
        query.prepare("UPDATE lib SET currentGenre = :currentGenre WHERE id = :id_lib");
        query.bindValue(":currentGenre", mLibs[g_idCurrentLib].uIdCurrentGenre);
        query.bindValue(":id_lib", g_idCurrentLib);
        query.exec();
    }

    // заполнение контрола дерева Книг по Авторам и Сериям из базы для выбранной библиотеки
    FillListBooks(listBooks, 0);
}

/*
    выбор (выделение) Группы в списке Групп
*/
void MainWindow::SelectGroup()
{
    ui->Books->clear();
    ExportBookListBtnEnabled(false);
    if (ui->GroupList->selectedItems().count() == 0)
        return;

    currentListForTag_ = qobject_cast<QObject*>(ui->GroupList);

    QListWidgetItem* selectedItem = ui->GroupList->selectedItems()[0];
    mLibs[g_idCurrentLib].uIdCurrentGroup = selectedItem->data(Qt::UserRole).toUInt();

    QSettings settings;
    if (settings.value("store_position", true).toBool()) {
        QSqlQuery query(QSqlDatabase::database("libdb"));
        query.setForwardOnly(true);
        query.prepare("UPDATE lib SET currentGroup = :currentGroup WHERE id = :id_lib");
        query.bindValue(":currentGroup", mLibs[g_idCurrentLib].uIdCurrentGroup);
        query.bindValue(":id_lib", g_idCurrentLib);
        query.exec();
    }

    // Выделение жирным выбранной Группы
    QFont font = ui->GroupList->font();
    QListWidgetItem* item = nullptr;
    for (int i = 0; i < ui->GroupList->count(); ++i) {
        item = ui->GroupList->item(i);
        if (item != selectedItem)
            font.setBold(false);
        else
            font.setBold(true);
        item->setFont(font);
    }

    // скроллинг до выделенной Группы
    ui->GroupList->scrollToItem(selectedItem);

    // заполнение контрола дерева Книг по Авторам и Сериям из базы для выбранной библиотеки
    QList<uint> listBooks = mLibs[g_idCurrentLib].mGroupBooksLink.values(mLibs[g_idCurrentLib].uIdCurrentGroup);
    FillListBooks(listBooks, 0);
}

/*
    выбор (выделение) Книги в списке Книг
*/
void MainWindow::SelectBook()
{
    if (ui->Books->selectedItems().count() == 0) {
        ExportBookListBtnEnabled(false);
        ui->Review->setHtml("");
        return;
    }

    currentListForTag_ = qobject_cast<QObject*>(ui->Books);

    ExportBookListBtnEnabled(true);
    QTreeWidgetItem* item = ui->Books->selectedItems()[0];
    if (item->type() != ITEM_TYPE_BOOK) {
        ui->btnOpenBook->setEnabled(false);
        ui->Review->setHtml("");
        return;
    }

    uint idBook = item->data(0, Qt::UserRole).toUInt();
    switch (ui->tabWidget->currentIndex()) {
    case 0: // Авторы
        mLibs[g_idCurrentLib].uIdCurrentBookForAuthor = idBook;
        break;
    case 1: // Серии
        mLibs[g_idCurrentLib].uIdCurrentBookForSeria = idBook;
        break;
    case 2: // Жанры
        mLibs[g_idCurrentLib].uIdCurrentBookForGenre = idBook;
        break;
    case 4: // Группы
        mLibs[g_idCurrentLib].uIdCurrentBookForGroup = idBook;
        break;
    }

    SBook &book = mLibs[g_idCurrentLib].mBooks[idBook];
    ui->btnOpenBook->setEnabled(true);
    //if (ui->splitter->sizes()[1] > 0) { // только, если панель данных книги видна.
    QBuffer buffer_book;
    QBuffer buffer_info;
    QDateTime book_date;
    QFileInfo fi = GetBookFile(buffer_book, buffer_info, idBook, false, &book_date);

    book_info bi;
    if (fi.fileName().isEmpty()) {
        GetBookInfo(bi, QByteArray(), "", true, idBook);
        QString file = book.sArchive.trimmed().isEmpty() ? book.sFile : book.sArchive;
        file = file.replace("\\", "/");
        bi.annotation = "<font color=\"red\">" + tr("Can't find file: %1").arg(file) + "</font>";
    }
    else {
        if (fi.fileName().right(3).toLower() == "fb2")
            GetBookInfo(bi, buffer_info.size() == 0 ? buffer_book.data() : buffer_info.data(), "fb2", false, idBook);
        else if (fi.fileName().right(4).toLower() == "epub")
            GetBookInfo(bi, buffer_book.data(), "epub", false, idBook);
        else
            GetBookInfo(bi, buffer_info.size() == 0 ? buffer_book.data() : buffer_info.data(), "fbd", false, idBook);
    }

    QString sSeria;
    QTreeWidgetItem* parent = item->parent();
    if (parent->type() == ITEM_TYPE_SERIA) {
        // если это серия
        QString sequenceName = parent->text(0);
        // удаление 'Sequence:' перед реальным названием серии, чтобы работала ссылка на эту Серию
        sequenceName = sequenceName.remove(0, sequenceName.indexOf(":") + 1).trimmed();
        sSeria = QString("<a href=seria_%3%1>%2</a>").arg(
            QString::number(/*-*/parent->data(0, Qt::UserRole).toLongLong()), sequenceName, sequenceName.left(1).toUpper()
        );
    }

    QString sAuthors;
    foreach(auto idAuthor, book.listIdAuthors) {
        QString sAuthor = mLibs[g_idCurrentLib].mAuthors[idAuthor].getName();
        sAuthors += (
            sAuthors.isEmpty() ? "" : "; ") +
            QString("<a href='author_%3%1'>%2</a>").arg(QString::number(idAuthor), sAuthor.replace(",", " "), sAuthor.left(1)
            );
    }
    QString sGenres;
    foreach(auto idGenre, book.listIdGenres) {
        QString sGenre = mGenre[idGenre].sName;
        sGenres += (
            sGenres.isEmpty() ? "" : "; ") +
            QString("<a href='genre_%3%1'>%2</a>").arg(QString::number(idGenre), sGenre, sGenre.left(1)
            );
    }

    QString sKeyWords = mLibs[g_idCurrentLib].mBooks[idBook].sKeywords.trimmed();

    QFile file_html(":/preview.html");
    file_html.open(QIODevice::ReadOnly);
    QString content(file_html.readAll());
    qint64 nSize = 0;
    QFileInfo fiArh;
    if (!fi.fileName().isEmpty()) {
        fiArh = fi;
        while (!fiArh.exists()) {
            fiArh.setFile(fiArh.absolutePath());
            if (fiArh.fileName().isEmpty())
                break;
        }
        nSize = fiArh.size();
    }
    QString sImgWidth = "220";
    content.replace("#annotation#", bi.annotation).
        replace("#title#", book.sName).
        replace("#width#", (bi.img.isEmpty() ? "0" : sImgWidth)).
        replace("#author#", sAuthors).
        replace("#genre#", sGenres).
        replace("#series#", sSeria).
        replace("#keywords#", sKeyWords).
        replace("#file_path#", fiArh.filePath()).
        replace("#file_size#", sizeToString(nSize)).
        replace("#file_data#", book_date.toString("dd.MM.yyyy hh:mm:ss")).
        replace("#file_name#", fi.fileName()).
        replace("#image#", bi.img);
    ui->Review->setHtml(content);
    //}
}

/*
    обновление контролов выбора языка книги панели инструментов для списка книг и вкладки поиска книг
*/
void MainWindow::UpdateBookLanguageControls(uint idLibrary)
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    SLib &currentLib = mLibs[idLibrary];

    ui->comboBoxLanguageFilter->blockSignals(true);
    ui->comboBoxFindLanguage->blockSignals(true);
    ui->comboBoxLanguageFilter->clear();
    ui->comboBoxLanguageFilter->addItem("*", -1);
    ui->comboBoxLanguageFilter->setCurrentIndex(0);
    ui->comboBoxFindLanguage->clear();
    ui->comboBoxFindLanguage->addItem("*", -1);
    ui->comboBoxFindLanguage->setCurrentIndex(0);

    QSqlQuery query(QSqlDatabase::database("libdb"));
    bool bIsAllLang = true;
    for (int iLang = 0; iLang < currentLib.vLaguages.size(); iLang++) {
        QString sLanguage = currentLib.vLaguages[iLang].toUpper();
        if (!sLanguage.isEmpty()) {
            ui->comboBoxLanguageFilter->addItem(sLanguage, iLang);
            ui->comboBoxFindLanguage->addItem(sLanguage, iLang);
            // язык фильтрации книг текущей библиотеки
            if (sLanguage == mLibs[idLibrary].sCurrentBookLanguage) {
                ui->comboBoxLanguageFilter->setCurrentIndex(ui->comboBoxLanguageFilter->count() - 1);
                idCurrentLanguage_ = iLang;
                bIsAllLang = false;
            }
        }
    }
    ui->comboBoxLanguageFilter->model()->sort(0);
    // сохранение языка фильтрации книг текущей библиотеки с id = idLibrary
    SaveCurrentBookLanguageFilter(idLibrary, ui->comboBoxLanguageFilter->currentText());
    ui->comboBoxLanguageFilter->blockSignals(false);
    ui->comboBoxFindLanguage->blockSignals(false);
    QApplication::restoreOverrideCursor();
}

/*
    обработчик экшена "Управления библиотеками" 
*/
void MainWindow::ManageLibrary()
{
    SaveLibPosition(g_idCurrentLib);
    AddLibrary al(this);
    al.exec();
    if (al.IsLibraryChanged()) {
        ui->Books->clear();
        QSettings settings;
        int nCurrentTab;
        if (settings.value("store_position", true).toBool())
        {
            // чтение из базы 'позиции' для текущей библиотеки с id = g_idCurrentLib
            nCurrentTab = LoadLibraryPosition(g_idCurrentLib);
        }
        loadBooksDataFromSQLiteToLibraryStructure(g_idCurrentLib);
        UpdateTagsMenu();
        UpdateBookLanguageControls(g_idCurrentLib);
        // заполнение комбобокса с форматами книг на вкладке Поиск
        FillFormatList(g_idCurrentLib);

        FillListWidgetAuthors(g_idCurrentLib);
        FillListWidgetSerials(g_idCurrentLib);
        FillTreeWidgetGenres(g_idCurrentLib);
        FillListWidgetGroups(g_idCurrentLib);

        searchChanged(ui->lineEditSearchString->text());
        setWindowTitle(AppName+(g_idCurrentLib<0||mLibs[g_idCurrentLib].name.isEmpty()?"":" - "+mLibs[g_idCurrentLib].name));
        FillLibrariesMenu(g_idCurrentLib);

        if (settings.value("store_position", true).toBool())
        {
            switch (nCurrentTab)
            {
            case 0:
                ui->btnAuthor->click();
                break;
            case 1:
                ui->btnSeries->click();
                break;
            case 2:
                ui->btnGenre->click();
                break;
            case 3:
                ui->btnSearch->click();
                break;
            case 4:
                ui->btnGroups->click();
                break;
            }
        }
        else
        {
            SelectFirstItemList(); // Выделение 1-го элемента списка Авторов или Серии
            FillListBooks();
        }
    }
}

/*
    Выделение 1-го элемента списка Авторов или Сери
*/
void MainWindow::SelectFirstItemList()
{
    switch (ui->tabWidget->currentIndex()) {
    case 0:
        if (ui->AuthorList->count() > 0)
            ui->AuthorList->item(0)->setSelected(true);
        break;
    case 1:
        if (ui->SeriaList->count() > 0)
            ui->SeriaList->item(0)->setSelected(true);
        break;
    case 2:
        ui->GenreList->setFocus();
        break;
    }
}
/*
    обработчик кнопки отображения списка Авторов
*/
void MainWindow::btnAuthorClick()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    ui->tabWidget->setCurrentIndex(0);
    ui->SearchFrame->setEnabled(true);
    ui->ExpandTreeGemresFrame->setVisible(false);
    ui->frameLang->setEnabled(true);
    ui->comboBoxLanguageFilter->setEnabled(true);
    ui->comboBoxTagFilter->setEnabled(true);
    SelectAuthor();
    QApplication::restoreOverrideCursor();
}

/*
    обработчик кнопки отображения списка Серий
*/
void MainWindow::btnSeriesClick()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    ui->tabWidget->setCurrentIndex(1);
    ui->SearchFrame->setEnabled(true);
    ui->ExpandTreeGemresFrame->setVisible(false);
    ui->frameLang->setEnabled(true);
    ui->comboBoxLanguageFilter->setEnabled(true);
    ui->comboBoxTagFilter->setEnabled(true);
    SelectSeria();
    QApplication::restoreOverrideCursor();
}

/*
    обработчик кнопки отображения дерева Жанров
*/
void MainWindow::btnGenresClick()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    ui->tabWidget->setCurrentIndex(2);
    ui->SearchFrame->setEnabled(false);
    ui->ExpandTreeGemresFrame->setVisible(true);
    ui->frameLang->setEnabled(false);
    ui->comboBoxLanguageFilter->setEnabled(true);
    ui->comboBoxTagFilter->setEnabled(true);
    SelectGenre();
    QApplication::restoreOverrideCursor();
}

/*
    обработчик кнопки отображения панели Поиска книг
*/
void MainWindow::btnPageSearchClick()
{
    ui->tabWidget->setCurrentIndex(3);
    ui->SearchFrame->setEnabled(false);
    ui->ExpandTreeGemresFrame->setVisible(false);
    ui->frameLang->setEnabled(false);
    ui->comboBoxLanguageFilter->setEnabled(false);
    ui->comboBoxTagFilter->setEnabled(false);
    ui->Books->clear();
    ui->labelFindBooks->setText("0");
    ExportBookListBtnEnabled(false);
}

/*
    запуск поиска Серии/Автора по нажатию кнопки на панели кнопок символов языка
*/
void MainWindow::LangBtnSearch()
{
    QToolButton *button = qobject_cast<QToolButton*>(sender());
    ui->lineEditSearchString->setText(button->text());
    searchChanged(ui->lineEditSearchString->text());
    FillListBooks();
}

void MainWindow::About()
{
    AboutDialog* dlg=new AboutDialog(this);
    dlg->exec();
    delete dlg;
}

/*
    обработчик изменения текста в контроле строки поиска
*/
void MainWindow::searchChanged(QString str)
{
    if(str.length()==0)
    {
        ui->lineEditSearchString->setText(lastSearchSymbol_);
        ui->lineEditSearchString->selectAll();
    }
    else
    {
        lastSearchSymbol_=ui->lineEditSearchString->text().left(1);
        if((ui->lineEditSearchString->text().left(1)=="*" || ui->lineEditSearchString->text().left(1)=="#" ) && ui->lineEditSearchString->text().length()>1)
        {
            ui->lineEditSearchString->setText(ui->lineEditSearchString->text().right(ui->lineEditSearchString->text().length()-1));
        }
        QList<QToolButton *> allButtons = findChildren<QToolButton *>();
        bool find=false;
        foreach(QToolButton *tb,allButtons)
        {
            if(tb->text()==ui->lineEditSearchString->text().left(1).toUpper())
            {
                find=true;
                tb->setChecked(true);
            }
        }
        if(!find)
            langBtnHash_->setChecked(true);
        FillListWidgetAuthors(g_idCurrentLib);
        FillListWidgetSerials(g_idCurrentLib);
    }
    tbClear_->setVisible(ui->lineEditSearchString->text().length()>1);
}

void MainWindow::searchClear()
{
    ui->lineEditSearchString->setText(ui->lineEditSearchString->text().left(1));
    searchChanged(ui->lineEditSearchString->text());
}


void MainWindow::HelpDlg()
{
    if(pHelpDlg_==nullptr)
        pHelpDlg_=new HelpDialog();
    pHelpDlg_->show();
}

/*
    создание и вызов контекстного меню для списков Авторов, Серий и Книг
*/
void MainWindow::ContextMenu(QPoint point)
{
    if(QObject::sender()==qobject_cast<QObject*>(ui->Books) && !ui->Books->itemAt(point))
        return;
    if(QObject::sender()==qobject_cast<QObject*>(ui->AuthorList) && !ui->AuthorList->itemAt(point))
        return;
    if(QObject::sender()==qobject_cast<QObject*>(ui->SeriaList) && !ui->SeriaList->itemAt(point))
        return;
    QMenu menu;
    currentListForTag_=QObject::sender();
    if(QObject::sender()==qobject_cast<QObject*>(ui->Books))
    {
        QMenu *save=menu.addMenu(tr("Save as"));
        foreach (QAction* i, ui->btnExport->menu()->actions())
        {
            QAction *action=new QAction(i->text(), this);
            action->setData(i->data().toInt());
            connect(action,SIGNAL(triggered()),this,SLOT(ExportAction()));
            save->addAction(action);
        }
        
        QTreeWidgetItem* bookItem = (ui->Books->selectedItems()[0]);
        if (bookItem->type() == ITEM_TYPE_BOOK)
        {
            // меню Оценки
            if (menu.actions().count() > 0)
                menu.addSeparator();
            menu.addMenu(menuRating_);
            menu.addSeparator();
            // меню книги Прочитано/Не прочитано
            menu.addMenu(menuReaded_);
            menu.addSeparator();
            // меню книги Группы
            QMenu* groups = menu.addMenu(tr("Add to Group"));
            QSqlQuery query(QSqlDatabase::database("libdb"));
            query.prepare("SELECT id, name, blocked FROM groups WHERE id_lib = :id_lib;");
            query.bindValue(":id_lib", g_idCurrentLib);
            if (!query.exec())
                qDebug() << query.lastError().text();
            while (query.next()) {
                QString name = query.value(1).toString();
                int id = query.value(0).toInt();
                QAction* actionGroup = new QAction(name, this);
                actionGroup->setData(id);
                connect(actionGroup, &QAction::triggered, this, &MainWindow::AddBookToGroupAction);
                groups->addAction(actionGroup);
            }
            if (ui->tabWidget->currentIndex() == 4 && ui->GroupList->selectedItems().count() > 0) {
                // action удаления выделенной книги из выделенной группы
                QAction* actionDeleteFromGroup = new QAction(tr("Remove from group"), this);
                uint idBooks = ui->Books->selectedItems()[0]->data(0, Qt::UserRole).toUInt();
                actionDeleteFromGroup->setData(QString::number(idBooks).toUInt());
                connect(actionDeleteFromGroup, &QAction::triggered, this, &MainWindow::DeleteBookFromGroupAction);
                menu.addAction(actionDeleteFromGroup);
            }
        }
    }
    if(menu.actions().count()>0)
        menu.addSeparator();
    if(bUseTag_)
        menu.addActions(menuTag_.actions());
    if(menu.actions().count()>0)
        menu.exec(QCursor::pos());
}

/*
    создание и вызов контекстного меню заголовков таблицы Книг
*/
void MainWindow::HeaderContextMenu(QPoint /*point*/)
{
    QMenu menu;
    QAction *action;

    action=new QAction(tr("Name"), this);
    action->setCheckable(true);
    action->setChecked(!ui->Books->isColumnHidden(0));
    connect(action,&QAction::triggered,this, [action, this]{ui->Books->setColumnHidden(0,!action->isChecked());});
    menu.addAction(action);

    action=new QAction(tr("No."), this);
    action->setCheckable(true);
    action->setChecked(!ui->Books->isColumnHidden(1));
    connect(action,&QAction::triggered,this, [action, this]{ShowHeaderCoulmn(1,"ShowName",!action->isChecked());});
    menu.addAction(action);

    action=new QAction(tr("Size"), this);
    action->setCheckable(true);
    action->setChecked(!ui->Books->isColumnHidden(2));
    connect(action,&QAction::triggered,this, [action, this]{ShowHeaderCoulmn(2,"ShowSize",!action->isChecked());});
    menu.addAction(action);

    action=new QAction(tr("Mark"), this);
    action->setCheckable(true);
    action->setChecked(!ui->Books->isColumnHidden(3));
    connect(action,&QAction::triggered,this, [action, this]{ShowHeaderCoulmn(3,"ShowMark",!action->isChecked());});
    menu.addAction(action);

    action=new QAction(tr("Import date"), this);
    action->setCheckable(true);
    action->setChecked(!ui->Books->isColumnHidden(4));
    connect(action,&QAction::triggered,this, [action, this]{ShowHeaderCoulmn(4,"ShowImportDate",!action->isChecked());});
    menu.addAction(action);

    action=new QAction(tr("Genre"), this);
    action->setCheckable(true);
    action->setChecked(!ui->Books->isColumnHidden(5));
    connect(action,&QAction::triggered,this, [action, this]{ShowHeaderCoulmn(5,"ShowGenre",!action->isChecked());});
    menu.addAction(action);

    action=new QAction(tr("Language"), this);
    action->setCheckable(true);
    action->setChecked(!ui->Books->isColumnHidden(6));
    connect(action,&QAction::triggered,this, [action, this]{ShowHeaderCoulmn(6,"ShowLanguage",!action->isChecked());});
    menu.addAction(action);

    action = new QAction(tr("Format"), this);
    action->setCheckable(true);
    action->setChecked(!ui->Books->isColumnHidden(7));
    connect(action, &QAction::triggered, this, [action, this] {ShowHeaderCoulmn(7, "ShowFormat", !action->isChecked()); });
    menu.addAction(action);

    action = new QAction(tr("Readed"), this);
    action->setCheckable(true);
    action->setChecked(!ui->Books->isColumnHidden(8));
    connect(action, &QAction::triggered, this, [action, this] {ShowHeaderCoulmn(8, "ShowReaded", !action->isChecked()); });
    menu.addAction(action);

    menu.exec(QCursor::pos());
}

void MainWindow::ShowHeaderCoulmn(int nColumn, const QString& sSetting, bool bHide)
{
    ui->Books->setColumnHidden(nColumn, bHide);
    QSettings settings;
    settings.beginGroup("Columns");
    settings.setValue(sSetting, !bHide);
}

/*
    обработчик клика мышкой на ссылках в описании Книги
*/
void MainWindow::onAnchorClicked(const QUrl& url)
{
    QString sPath = url.path();
    if (sPath.startsWith("author_"))
    {
        MoveToAuthor(sPath.right(sPath.length() - 8).toLongLong(), sPath.mid(7, 1).toUpper());
    }
    else if (sPath.startsWith("genre_"))
    {
        MoveToGenre(sPath.right(sPath.length() - 7).toLongLong());
    }
    else if (sPath.startsWith("seria_"))
    {
        MoveToSeria(sPath.right(sPath.length() - 7).toLongLong(), sPath.mid(6, 1).toUpper());
    }
}

/*
    переход к выбранному Автору в списке Авторов по клику на Авторе-ссылке в описании Книги
*/
void MainWindow::MoveToAuthor(qlonglong id, QString FirstLetter)
{
    ui->lineEditSearchString->setText(/*id<0?Item->text(0).left(1).toUpper():*/FirstLetter);
    ui->btnAuthor->setChecked(true);
    searchChanged(FirstLetter);
    btnAuthorClick();
    ui->AuthorList->clearSelection();
    for (int i = 0; i < ui->AuthorList->count(); i++)
    {
        if (ui->AuthorList->item(i)->data(Qt::UserRole).toLongLong() == id)
        {
            ui->AuthorList->item(i)->setSelected(true);
            ui->AuthorList->scrollToItem(ui->AuthorList->item(i));
            SelectAuthor();
            break;
        }
    }
}

/*
    переход к выбранной Серии в списке Серий по клику на Серии-ссылке в описании Книги
*/
void MainWindow::MoveToSeria(qlonglong id,QString FirstLetter)
{
    ui->lineEditSearchString->setText(FirstLetter);
    ui->btnSeries->setChecked(true);
    btnSeriesClick();
    ui->SeriaList->clearSelection();
    for (int i=0;i<ui->SeriaList->count();i++)
    {
        if(ui->SeriaList->item(i)->data(Qt::UserRole).toLongLong()==id)
        {
            ui->SeriaList->item(i)->setSelected(true);
            ui->SeriaList->scrollToItem(ui->SeriaList->item(i));
            SelectSeria();
            return;
        }
    }
}

/*
    переход к выбранному Жанру в дереве Жанров по клику на Жанре-ссылке в описании Книги
*/
void MainWindow::MoveToGenre(qlonglong id)
{
    ui->btnGenre->setChecked(true);
    btnGenresClick();
    ui->GenreList->clearSelection();
    for (int i=0;i<ui->GenreList->topLevelItemCount();i++)
    {
        for (int j=0;j<ui->GenreList->topLevelItem(i)->childCount();j++)
        {
            if(ui->GenreList->topLevelItem(i)->child(j)->data(0,Qt::UserRole).toLongLong()==id)
            {
                ui->GenreList->topLevelItem(i)->child(j)->setSelected(true);
                ui->GenreList->scrollToItem(ui->GenreList->topLevelItem(i)->child(j));
                SelectGenre();
                return;
            }
        }
    }
}

void MainWindow::ProcPath(QString path,QStringList *book_list)
{
#ifdef Q_OS_WIN
    while(path.left(1)=="/")
        path=path.right(path.length()-1);
#endif
    QFileInfo fi(path);
    if(fi.isFile())
    {
        *book_list<<path;
    }
    else if(fi.isDir())
    {
        QDir dir(path);
        QFileInfoList info_list = dir.entryInfoList(QDir::NoSymLinks|QDir::NoDotAndDotDot|QDir::Readable|QDir::Files|QDir::Dirs|QDir::Readable);
        QList<QFileInfo>::iterator iter=info_list.begin();
        for(iter=info_list.begin();iter != info_list.end();iter++)
        {
            ProcPath(iter->absoluteFilePath(),book_list);
        }
    }
}

/*
    Заполнение меню списка Библиотек
*/
void MainWindow::FillLibrariesMenu(uint idLibrary)
{
    if (!db_is_open)
        return;
    QMenu *lib_menu = new QMenu(this);
    auto i = mLibs.constBegin();
    while (i != mLibs.constEnd()) {
        if (i.key() != -1) {
            QAction* action = new QAction(i->name, this);
            action->setData(i.key());
            action->setCheckable(true);
            lib_menu->insertAction(nullptr, action);
            connect(action, SIGNAL(triggered()), this, SLOT(SelectLibrary()));
            action->setChecked(i.key() == idLibrary);
        }
        ++i;
    }
    if (lib_menu->actions().count() > 0) {
        ui->actionLibraries->setMenu(lib_menu);
        ui->actionLibraries->setEnabled(true);
    }
}

/*
    заполнение контрола списка Авторов из базы для выбранной библиотеки
*/
void MainWindow::FillListWidgetAuthors(uint idLibrary)
{
    qint64 t_start = QDateTime::currentMSecsSinceEpoch();
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    const bool wasBlocked = ui->AuthorList->blockSignals(true);
    QListWidgetItem *item;
    ui->AuthorList->clear();
    SLib &currentLib = mLibs[idLibrary];
    QListWidgetItem *selectedItem = nullptr;
    QString sSearch = ui->lineEditSearchString->text();

    auto i = currentLib.mAuthors.constBegin();
    while (i!=currentLib.mAuthors.constEnd()){
        if (sSearch == "*" || (sSearch=="#" && !i->getName().left(1).contains(QRegExp("[A-Za-zа-яА-ЯЁё]"))) || i->getName().startsWith(sSearch, Qt::CaseInsensitive)) {
            QList<uint> booksId = currentLib.mAuthorBooksLink.values(i.key());
            int count = 0;
            foreach (uint idBook, booksId) {
                SBook &book = currentLib.mBooks[idBook];
                if (IsMatchingFilterConditions(idLibrary, book))
                    count++;
            }
            if (count > 0) {
                item = new QListWidgetItem(QString("%1 (%2)").arg(i->getName()).arg(count));
                item->setData(Qt::UserRole, i.key());
                if (bUseTag_)
                    item->setIcon(GetTagFromTagsPicList(i->nTag));
                ui->AuthorList->addItem(item);
                if (mLibs[idLibrary].uIdCurrentAuthor == i.key()) {
                    item->setSelected(true);
                    selectedItem = item;
                }
            }
        }
        ++i;
    }
    if (selectedItem != nullptr)
        ui->AuthorList->scrollToItem(selectedItem);

    ui->AuthorList->blockSignals(wasBlocked);
    qint64 t_end = QDateTime::currentMSecsSinceEpoch();
    qDebug() << "FillListWidgetAuthors " << t_end-t_start << "msec";
    QApplication::restoreOverrideCursor();
}

/*
    заполнение контрола списка Серий из базы для выбранной библиотеки
*/
void MainWindow::FillListWidgetSerials(uint idLibrary)
{
    qint64 t_start = QDateTime::currentMSecsSinceEpoch();
    const bool wasBlocked = ui->SeriaList->blockSignals(true);
    ui->SeriaList->clear();
    QString sSearch = ui->lineEditSearchString->text();

    QMap<uint,uint> mCounts;
    auto iBook = mLibs[idLibrary].mBooks.constBegin();
    while(iBook!=mLibs[idLibrary].mBooks.constEnd()){
        if(IsMatchingFilterConditions(idLibrary , *iBook) &&
            (sSearch == "*" || (sSearch=="#" && !mLibs[idLibrary].mSerials[iBook->idSerial].sName.left(1).contains(QRegExp("[A-Za-zа-яА-ЯЁё]"))) || mLibs[g_idCurrentLib].mSerials[iBook->idSerial].sName.startsWith(sSearch, Qt::CaseInsensitive)))
        {
            if(mCounts.contains(iBook->idSerial))
                mCounts[iBook->idSerial]++;
            else
                mCounts[iBook->idSerial] = 1;

        }
        ++iBook;
    }

    QListWidgetItem *item;
    auto iSerial = mCounts.constBegin();
    while (iSerial != mCounts.constEnd()) {
        QString SeriaName = mLibs[idLibrary].mSerials[iSerial.key()].sName;
        //QBrush Brush; Brush = SeriaName != "" ? Qt::black : Qt::darkMagenta;
        item = new QListWidgetItem(QString("%1 (%2)").arg(SeriaName).arg(iSerial.value()));
        //item->setForeground(Brush);
        item->setData(Qt::UserRole, iSerial.key());
        if (bUseTag_)
            item->setIcon(GetTagFromTagsPicList(mLibs[idLibrary].mSerials[iSerial.key()].nTag));
        ui->SeriaList->addItem(item);
        if (iSerial.key() == mLibs[idLibrary].uIdCurrentSeria) {
            item->setSelected(true);
            ui->SeriaList->scrollToItem(item);
        }
        ++iSerial;
    }

    ui->SeriaList->blockSignals(wasBlocked);
    qint64 t_end = QDateTime::currentMSecsSinceEpoch();
    qDebug() << "FillListWidgetSerials " << t_end-t_start << "msec";
}

/*
    заполнение контрола дерева Жанров из базы для выбранной библиотеки
*/
void MainWindow::FillTreeWidgetGenres(uint idLibrary)
{
    qint64 t_start = QDateTime::currentMSecsSinceEpoch();
    const bool wasBlocked = ui->GenreList->blockSignals(true);
    ui->GenreList->clear();
    ui->comboBoxFindGenre->clear();
    ui->comboBoxFindGenre->addItem("*", 0);
    QFont bold_font(ui->AuthorList->font());
    bold_font.setBold(true);

    QMap<uint,uint> mCounts;
    auto iBook = mLibs[idLibrary].mBooks.constBegin();
    while (iBook != mLibs[idLibrary].mBooks.constEnd()) {
        if (IsMatchingFilterConditions(idLibrary , *iBook)) {
            foreach (uint iGenre, iBook->listIdGenres) {
                if(mCounts.contains(iGenre))
                    mCounts[iGenre]++;
                else
                    mCounts[iGenre] = 1;
            }
        }
        ++iBook;
    }

    QSqlQuery query(QSqlDatabase::database("libdb"));
    query.setForwardOnly(true);
    QString childGenresCount;
    QMap<uint, QTreeWidgetItem*> mTopGenresItem;
    auto iGenre = mGenre.constBegin();
    while (iGenre != mGenre.constEnd()) {
        QTreeWidgetItem *item;
        if (iGenre->idParrentGenre == 0 && !mTopGenresItem.contains(iGenre.key())) {
            // определение числа книг в всех дочерних жанрах данной группы Жанров
            query.prepare("SELECT COUNT(book_genre.id_book) FROM lib, genre, book_genre WHERE lib.id = :id_lib AND genre.id_parent = :id_parent AND genre.id = book_genre.id_genre AND book_genre.id_lib = lib.id;");
            query.bindValue(":id_lib", idLibrary);
            query.bindValue(":id_parent", iGenre.key());
            if (!query.exec())
                qDebug() << query.lastError().text();
            query.next();
            uint allGenresCountForGroup = query.value(0).toUInt();
            childGenresCount = allGenresCountForGroup  > 0
                ? QString("%1 (%2)").arg(iGenre->sName).arg(allGenresCountForGroup)
                : iGenre->sName;
            item = new QTreeWidgetItem(ui->GenreList);
            item->setFont(0, bold_font);
            item->setText(0, childGenresCount);
            item->setData(0, Qt::UserRole, iGenre.key());
            item->setExpanded(false);
            mTopGenresItem[iGenre.key()] = item;
        } else {
            if (mCounts.contains(iGenre.key())) {
                if (!mTopGenresItem.contains(iGenre->idParrentGenre)) {
                    QTreeWidgetItem *itemTop = new QTreeWidgetItem(ui->GenreList);
                    itemTop->setFont(0, bold_font);
                    itemTop->setText(0, mGenre[iGenre->idParrentGenre].sName);
                    itemTop->setData(0, Qt::UserRole, iGenre->idParrentGenre);
                    itemTop->setExpanded(false);
                    mTopGenresItem[iGenre->idParrentGenre] = itemTop;
                }
                item = new QTreeWidgetItem(mTopGenresItem[iGenre->idParrentGenre]);
                item->setText(0, QString("%1 (%2)").arg(iGenre->sName).arg(mCounts[iGenre.key()]));
                item->setData(0, Qt::UserRole,iGenre.key());
                if (iGenre.key() == mLibs[idLibrary].uIdCurrentGenre) {
                    item->setSelected(true);
                    ui->GenreList->scrollToItem(item);
                }
            }
        }
        ++iGenre;
    }

    // заполнение комбобокса на вкладке Жанров 'Поиск' (дерево с 2-мя уровнями ветвей, обход без рекурсии)
    int topCount = ui->GenreList->topLevelItemCount();
    QStandardItemModel* model = (QStandardItemModel*)ui->comboBoxFindGenre->model();
    for (int i = 0; i < topCount; i++) {
        QTreeWidgetItem* topLevelItem = ui->GenreList->topLevelItem(i);
        int childCount = topLevelItem->childCount();
        uint topLevelKey = topLevelItem->data(0, Qt::UserRole).toUInt();
        auto topLevelGenre = mGenre.find(topLevelKey);
        if (childCount == 0) {
            ui->comboBoxFindGenre->addItem(topLevelGenre.value().sName, topLevelKey);
            model->item(ui->comboBoxFindGenre->count()-1)->setFont(bold_font);
        } else {
            ui->comboBoxFindGenre->addItem(topLevelGenre.value().sName, topLevelKey);
            model->item(ui->comboBoxFindGenre->count()-1)->setFont(bold_font);
            for (int j = 0; j < childCount; j++) {
                QTreeWidgetItem* childItem = topLevelItem->child(j);
                uint childLevelKey = childItem->data(0, Qt::UserRole).toUInt();
                auto childGenre = mGenre.find(childLevelKey);
                ui->comboBoxFindGenre->addItem("   " + childGenre.value().sName, childLevelKey);
            }
        }
    }

    ui->GenreList->blockSignals(wasBlocked);
    qint64 t_end = QDateTime::currentMSecsSinceEpoch();
    qDebug() << "FillTreeWidgetGenres " << t_end-t_start << "msec";
}

/*
    заполнение контрола списка Групп из базы для выбранной библиотеки
*/
void MainWindow::FillListWidgetGroups(uint idLibrary)
{
    qint64 t_start = QDateTime::currentMSecsSinceEpoch();
    const bool wasBlocked = ui->GroupList->blockSignals(true);
    ui->GroupList->clear();
    SLib& currentLib = mLibs[idLibrary];

    QSqlQuery query(QSqlDatabase::database("libdb"));
    QListWidgetItem* item = nullptr;
    QListWidgetItem* selectedItem = nullptr;
    QList<QListWidgetItem*> blockedItemList;
    QHash<uint, Group>::const_iterator iGroup = currentLib.mGroups.constBegin();
    while (iGroup != currentLib.mGroups.constEnd()) {
        QPixmap pixmap = QPixmap();
        uint idGroup = iGroup.key();
        QString GroupName = iGroup->getName();
        int booksCountInGroup = currentLib.mGroupBooksLink.values(idGroup).count();
        // иконки Групп из базы
        query.prepare("SELECT icon FROM groups WHERE id_lib=:id_lib AND id = :idGroup;");
        query.bindValue(":idGroup", idGroup);
        query.bindValue(":id_lib", idLibrary);
        if (!query.exec())
            qDebug() << query.lastError().text();
        query.first();
        pixmap.loadFromData(query.value(0).toByteArray());
        
        if (booksCountInGroup > 0)
            item = new QListWidgetItem(QString("%1 (%2)").arg(GroupName).arg(booksCountInGroup));
        else
            item = new QListWidgetItem(GroupName);
        item->setData(Qt::UserRole, idGroup);
        item->setIcon(QIcon(pixmap));
        
        if (iGroup->isBlocked())
            blockedItemList << item;
        else
            ui->GroupList->addItem(item);
        
        // выделенная Группа по uIdCurrentGroup
        if (idGroup == mLibs[idLibrary].uIdCurrentGroup)
            selectedItem = item;
        
        ++iGroup;
    }

    // добавление заблокированных групп в начало списка
    for (int i = 0; i != blockedItemList.count(); ++i) {
        blockedItemList[i]->setBackgroundColor(QColor(200, 162, 200, 127));
        ui->GroupList->insertItem(0, blockedItemList[i]);
    }

    // скроллинг до Группы, выделенной по uIdCurrentGroup
    if (selectedItem != nullptr) {
        selectedItem->setSelected(true);
        ui->GroupList->scrollToItem(selectedItem);
    }

    ui->GroupList->blockSignals(wasBlocked);
    qint64 t_end = QDateTime::currentMSecsSinceEpoch();
    qDebug() << "FillListWidgetGroups " << t_end - t_start << "msec";
}

/*
    выбор (выделение) Автора, Серии, Жанра, в зависимости от активного виджета списков Авторов, Серий или Жанров
*/
void MainWindow::FillListBooks()
{
    ui->Books->clear();
    switch(ui->tabWidget->currentIndex()){
        case 0:
            SelectAuthor();
            break;

        case 1:
            SelectSeria();
            break;

        case 2:
            SelectGenre();
            break;

        case 4:
            SelectGroup();
            break;

    }
}

/*
    заполнение контрола дерева Книг по Авторам и Сериям из базы для выбранной библиотеки
*/
void MainWindow::FillListBooks(QList<uint> listBook, uint idCurrentAuthor)
{
    qint64 t_start = QDateTime::currentMSecsSinceEpoch();
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

    QFont bold_font(ui->Books->font());
    bold_font.setBold(true);
    TreeBookItem* ScrollItem = nullptr;

    TreeBookItem* item_seria = nullptr;
    TreeBookItem* item_book;
    TreeBookItem* item_author;
    QMap<uint, TreeBookItem*> mAuthors;
    QMultiMap<uint, TreeBookItem*> mSerias;

    const bool wasBlocked = ui->Books->blockSignals(true);
    ui->Books->clear();

    foreach (uint idBook, listBook) {
        SBook &book = mLibs[g_idCurrentLib].mBooks[idBook];
        if (IsMatchingFilterConditions(g_idCurrentLib, book)) {
            uint idSerial = book.idSerial;
            uint idAuthor;
            if (idCurrentAuthor > 0)
                idAuthor = idCurrentAuthor;
            else
                idAuthor = book.idFirstAuthor;
            if (!mAuthors.contains(idAuthor)) {
                item_author = new TreeBookItem(ui->Books, ITEM_TYPE_AUTHOR);
                item_author->setText(0, mLibs[g_idCurrentLib].mAuthors[idAuthor].getName());
                item_author->setExpanded(true);
                item_author->setFont(0, bold_font);
                item_author->setCheckState(0, Qt::Unchecked);
                item_author->setData(0, Qt::UserRole,idAuthor);
                if(bUseTag_)
                    item_author->setIcon(0, GetTagFromTagsPicList(mLibs[g_idCurrentLib].mAuthors[idAuthor].nTag));
                mAuthors[idAuthor] = item_author;
            } else
                item_author = mAuthors[idAuthor];

            if (idSerial > 0) {
                auto iSerial = mSerias.find(idSerial);
                while (iSerial != mSerias.constEnd()) {
                    item_seria = iSerial.value();
                    if (item_seria->parent()->data(0, Qt::UserRole) == idAuthor)
                        break;
                    ++iSerial;
                }
                if (iSerial == mSerias.constEnd()) {
                    item_seria = new TreeBookItem(mAuthors[idAuthor], ITEM_TYPE_SERIA);
                    QString SeriaName = mLibs[g_idCurrentLib].mSerials[idSerial].sName;
                    QString NewSeriaName = tr("Sequence") + ": " + SeriaName;
                    item_seria->setText(0, NewSeriaName);
                    item_author->addChild(item_seria);
                    item_seria->setExpanded(true);
                    item_seria->setFont(0, bold_font);
                    item_seria->setCheckState(0, Qt::Unchecked);
                    item_seria->setData(0, Qt::UserRole,idSerial);
                    if (bUseTag_)
                        item_seria->setIcon(0, GetTagFromTagsPicList(mLibs[g_idCurrentLib].mSerials[idSerial].nTag));

                    mSerias.insert(idSerial,item_seria);

                }
                item_book = new TreeBookItem(item_seria, ITEM_TYPE_BOOK);
            } else
                item_book = new TreeBookItem(item_author, ITEM_TYPE_BOOK);

            item_book->setCheckState(0, Qt::Unchecked);
            item_book->setData(0, Qt::UserRole,idBook);
            if (bUseTag_)
                item_book->setIcon(0, GetTagFromTagsPicList(book.nTag));

            item_book->setText(0, book.sName);
            if (book.numInSerial > 0) {
                item_book->setText(1, QString::number(book.numInSerial));
                item_book->setTextAlignment(1, Qt::AlignRight);
            }

            if (book.nSize>0)
                item_book->setText(2, sizeToString(book.nSize));
            item_book->setTextAlignment(2, Qt::AlignRight);

            QPixmap pix(
                ":/icons/img/icons/stars/" + QString::number(book.nStars).trimmed() + QString("star%1.png").arg(app->devicePixelRatio() >= 2 ? "@2x" : "")
            );
            pix.setDevicePixelRatio(app->devicePixelRatio());
            item_book->setData(3, Qt::DecorationRole,pix);

            item_book->setText(4, book.date.toString("dd.MM.yyyy"));
            item_book->setTextAlignment(4, Qt::AlignCenter);

            item_book->setText(5, mGenre[book.listIdGenres.first()].sName);
            item_book->setTextAlignment(5, Qt::AlignLeft);

            item_book->setText(6, mLibs[g_idCurrentLib].vLaguages[book.idLanguage]);
            item_book->setTextAlignment(6, Qt::AlignCenter);

            item_book->setText(7, book.sFormat);
            item_book->setTextAlignment(7, Qt::AlignCenter);

            item_book->setTextAlignment(8, Qt::AlignCenter);
            // пометка ячейки статуса 'Прочитано'
            MarkReadedBook(item_book, book.bReaded);

            if (book.bDeleted) {
                QBrush brush(QColor::fromRgb(196, 96, 96));
                for (int i = 0; i != item_book->columnCount(); ++i)
                    item_book->setForeground(i, brush);
            }

            uInt idCurrentBook = 0;
            switch (ui->tabWidget->currentIndex())
            {
            case 0: // Авторы
                idCurrentBook = mLibs[g_idCurrentLib].uIdCurrentBookForAuthor;
                break;
            case 1: // Серии
                idCurrentBook = mLibs[g_idCurrentLib].uIdCurrentBookForSeria;
                break;
            case 2: // Жанры
                idCurrentBook = mLibs[g_idCurrentLib].uIdCurrentBookForGenre;
                break;
            case 4: // Группы
                idCurrentBook = mLibs[g_idCurrentLib].uIdCurrentBookForGroup;
                break;
            }

            if (idBook == idCurrentBook)
                ScrollItem = item_book;
        }
    }
    if (ScrollItem) {
        ScrollItem->setSelected(true);
        ui->Books->scrollToItem(ScrollItem);
    }
    ui->Books->setFocus();
    SelectBook();

    ui->Books->blockSignals(wasBlocked);
    qint64 t_end = QDateTime::currentMSecsSinceEpoch();
    qDebug() << "FillListBooks " << t_end-t_start << "msec";

    QApplication::restoreOverrideCursor();
}

/*
    выполняются ли условия, чтобы книга оказалась в списке (фильтрация Языка и Метки, отображения удаленных книг)
*/
bool MainWindow::IsMatchingFilterConditions(uint idLibrary, const SBook &book) const
{
    int current_tag=ui->comboBoxTagFilter->itemData(ui->comboBoxTagFilter->currentIndex()).toInt();
    uint idSerial=book.idSerial;

    return (idCurrentLanguage_ == -1 || idCurrentLanguage_ == book.idLanguage)
            &&(bShowDeleted_ || !book.bDeleted) &&
            (!bUseTag_ || current_tag==0 || current_tag==book.nTag
             ||(idSerial>0 && mLibs[idLibrary].mSerials[idSerial].nTag == current_tag)
             ||(mLibs[idLibrary].mAuthors[book.idFirstAuthor].nTag == current_tag));
}

void MainWindow::dropEvent(QDropEvent *ev)
{
    if (mode == MODE_LIBRARY)
        pDropForm_->hide();
    QList<QUrl> urls = ev->mimeData()->urls();
    QStringList book_list;
    foreach (QUrl url, urls) {
        ProcPath(url.path(), &book_list);
    }
    if (book_list.count()) {
        ExportDlg dlg(this);
        int id = pDropForm_->get_command(ev->pos());
        if (id < 0) {
            pDropForm_->get_command(QPoint(-1, -1));
            return;
        }
        dlg.exec(book_list, SetCurrentExportSettings(id));
    }
    pDropForm_->get_command(QPoint(-1, -1));
}

void MainWindow::DeleteDropForm()
{
    if (pDropForm_ != nullptr) {
        if (pDropForm_->isHidden()) {
            delete pDropForm_;
            pDropForm_ = nullptr;
        }
    }
}

void MainWindow::ShowDropForm()
{
    if (pDropForm_ == nullptr)
        pDropForm_ = new DropForm(this);
    if (mode == MODE_CONVERTER) {
        pDropForm_->setFixedWidth(ui->drop_buttons->rect().width());
        pDropForm_->setFixedHeight(ui->drop_buttons->rect().height());
        pDropForm_->move(ui->drop_buttons->mapToGlobal(ui->drop_buttons->pos()) - this->mapToGlobal(QPoint(0, 0)));
    }
    else {
        pDropForm_->setFixedWidth(rect().width() / 10 * 9);
        pDropForm_->setFixedHeight(rect().height() / 10 * 9);
        pDropForm_->move(QPoint(rect().width() / 20, rect().height() / 20));
    }
    QStringList cmd;
    foreach (QAction* action, ui->btnExport->menu()->actions()) {
        cmd<<action->text();
    }
    pDropForm_->AddCommand(cmd);
    pDropForm_->setWindowFlags(Qt::WindowStaysOnTopHint);
    pDropForm_->show();
    pDropForm_->activateWindow();
    pDropForm_->raise();
}

void MainWindow::dragEnterEvent(QDragEnterEvent* ev)
{
    if (ev->mimeData()->urls().count() > 0) {
        ev->accept();
        if (mode == MODE_LIBRARY) {
            DeleteDropForm();
            ShowDropForm();
        }
    }
    else {
        ev->setAccepted(false);
        if (mode == MODE_LIBRARY) {
            if (pDropForm_ != nullptr)
                pDropForm_->hide();
        }
    }
}
void MainWindow::dragMoveEvent(QDragMoveEvent *ev)
{
    pDropForm_->switch_command(ev->pos());
}

void MainWindow::dragLeaveEvent(QDragLeaveEvent *)
{
    if(mode==MODE_LIBRARY)
        pDropForm_->hide();
}

/*
    обновление контролов меню экспорта книг на панели инструментов
*/
void MainWindow::UpdateExportMenu()
{
    QSettings settings;
    int defaultID = -1;
    if (ui->btnExport->defaultAction())
        defaultID = ui->btnExport->defaultAction()->data().toInt();
    else
        defaultID = settings.value("DefaultExport", -1).toInt();
    QMenu* menu=ui->btnExport->menu();
    if (menu) {
        ui->btnExport->menu()->clear();
    }
    else {
        menu = new QMenu(this);
        ui->btnExport->setMenu(menu);
    }
    ui->btnExport->setDefaultAction(nullptr);
    int count = settings.beginReadArray("export");
    for (int i = 0; i < count; i++) {
        settings.setArrayIndex(i);
        QAction *action = new QAction(settings.value("ExportName").toString(), this);
        action->setData(i);
        menu->addAction(action);
        if (settings.value("Default").toBool() || (i == defaultID && !ui->btnExport->defaultAction())) {
            ui->btnExport->setDefaultAction(action);
        }
    }
    settings.endArray();
    if (count == 0) {
       QAction *action=new QAction(tr("Send to ..."), this);
       action->setData(-1);
       menu->addAction(action);
       ui->btnExport->setDefaultAction(action);
    }
    if (menu->actions().count() == 0) {
        return;
    }
    if (!ui->btnExport->defaultAction()) {
        ui->btnExport->setDefaultAction(menu->actions()[0]);
    }
    foreach (QAction *action, menu->actions()) {
        connect(action, SIGNAL(triggered()), this, SLOT(ExportAction()));
    }
    QFont font(ui->btnExport->defaultAction()->font());
    font.setBold(true);
    ui->btnExport->defaultAction()->setFont(font);
    ui->btnExport->setIcon(QIcon(":/icons/img/icons/Streamline.png"));
    ui->btnExport->setEnabled(ui->Books->selectedItems().count() > 0);
}

/*
    экспорт выделенных книг
*/
void MainWindow::ExportAction()
{
    int id = qobject_cast<QAction*>(sender())->data().toInt();
    QSettings settings;
    int count = settings.beginReadArray("export");
    if (count > 1 && ui->btnExport->defaultAction()) {
        settings.setArrayIndex(ui->btnExport->defaultAction()->data().toInt());
        if (!settings.value("Default").toBool()) {
            ui->btnExport->setDefaultAction(qobject_cast<QAction*>(sender()));
            QList<QAction*> actions = ui->btnExport->menu()->actions();
            foreach (QAction* action, actions) {
                QFont font(action->font());
                font.setBold(action == ui->btnExport->defaultAction());
                action->setFont(font);
            }
            ui->btnExport->setIcon(QIcon(":/icons/img/icons/Streamline.png"));
        }
    }
    settings.endArray();
    SendType type=SetCurrentExportSettings(id);
    if (type == ST_Device)
        SendToDevice();
    else
        SendMail();
}

/*
    установка рейтинга (оценки) книги
*/
void MainWindow::RatingAction()
{
    QImage image;
    QTreeWidgetItem* bookItem = (ui->Books->selectedItems()[0]);
    uint id = bookItem->data(0, Qt::UserRole).toUInt();
    uchar star_id = static_cast<uchar>(qobject_cast<QAction*>(QObject::sender())->data().toInt());
    QSqlQuery query(QSqlDatabase::database("libdb"));
    switch (bookItem->type()) {
    case ITEM_TYPE_BOOK:
        query.prepare("UPDATE book set star=:star where id=:id");
        query.bindValue(":star", star_id);
        query.bindValue(":id", id);
        query.exec();
        mLibs[g_idCurrentLib].mBooks[id].nStars = star_id;
        image.load(":/icons/img/icons/stars/" + QString::number(mLibs[g_idCurrentLib].mBooks[id].nStars).trimmed() + QString("star%1.png").arg(app->devicePixelRatio() >= 2 ? "@2x" : ""));
        image.setDevicePixelRatio(app->devicePixelRatio());
        bookItem->setData(3, Qt::DecorationRole, image);
        break;

    default:
        break;
    }
}

/*
    установка признака прочитана / не прочитана книга
*/
void MainWindow::ReadedAction()
{
    QTreeWidgetItem* bookItem = (ui->Books->selectedItems()[0]);
    uint id = bookItem->data(0, Qt::UserRole).toUInt();
    int idReaded = static_cast<int>(qobject_cast<QAction*>(QObject::sender())->data().toInt());
    QSqlQuery query(QSqlDatabase::database("libdb"));
    switch (bookItem->type()) {
    case ITEM_TYPE_BOOK:
        // пометка ячейки статуса 'Прочитано'
        MarkReadedBook(bookItem, idReaded);
        query.prepare("UPDATE book SET readed = :readed WHERE id=:id");
        query.bindValue(":readed", idReaded);
        query.bindValue(":id", id);
        query.exec();
        mLibs[g_idCurrentLib].mBooks[id].bReaded = idReaded;
        break;

    default:
        break;
    }
}

/*
    обработчик переключения в режим конвертера из меню
*/
void MainWindow::on_actionSwitch_to_convert_mode_triggered()
{
    QSettings settings;
    if (mode == MODE_LIBRARY) {
        settings.setValue("MainWnd/geometry", saveGeometry());
        settings.setValue("MainWnd/windowState", saveState());
        settings.setValue("MainWnd/tab/geometry", ui->tabWidget->saveGeometry());
        settings.setValue("MainWnd/tab/geometry", ui->splitter->saveState());
        settings.setValue("MainWnd/books/geometry", ui->splitter_2->saveState());
        settings.setValue("MainWnd/books_head/geometry", ui->Books->header()->saveState());
    }
    ui->stackedWidget->setCurrentWidget(ui->pageConvert);
    ui->actionSwitch_to_library_mode->setVisible(true);
    ui->actionSwitch_to_convert_mode->setVisible(false);

    ui->actionCheck_uncheck->setVisible(false);
    ui->actionLibraries->setVisible(false);
    ui->actionAddLibrary->setVisible(false);
    ui->actionNew_library_wizard->setVisible(false);

    ui->actionMarkDeletedBooks->setVisible(false);
    ui->actionDatabaseOptimization->setVisible(false);

    setWindowTitle(AppName);
    mode = MODE_CONVERTER;

    setMinimumSize(200, 200);
    if (settings.contains("MainWndConvertMode/geometry"))
        restoreGeometry(settings.value("MainWndConvertMode/geometry").toByteArray());

    settings.setValue("ApplicationMode", mode);
    if (pDropForm_ != nullptr) {
        pDropForm_->hide();
        DeleteDropForm();
    }
    ShowDropForm();
}

/*
    обработчик переключения в режим библиотеки из меню
*/
void MainWindow::on_actionSwitch_to_library_mode_triggered()
{
    QSettings settings;
    if (mode == MODE_CONVERTER) {
        settings.setValue("MainWndConvertMode/geometry", saveGeometry());
    }
    mode = MODE_LIBRARY;
    if (pDropForm_ != nullptr) {
        delete pDropForm_;
        pDropForm_ = nullptr;
    }
    ui->stackedWidget->setCurrentWidget(ui->pageLabrary);
    ui->actionSwitch_to_library_mode->setVisible(false);
    ui->actionSwitch_to_convert_mode->setVisible(true);

    ui->actionCheck_uncheck->setVisible(true);
    ui->actionLibraries->setVisible(true);
    ui->actionAddLibrary->setVisible(true);
    ui->actionNew_library_wizard->setVisible(true);

    ui->actionMarkDeletedBooks->setVisible(true);
    ui->actionDatabaseOptimization->setVisible(true);

    setWindowTitle(
        AppName + (g_idCurrentLib < 0 || mLibs[g_idCurrentLib].name.isEmpty() ? "" : " - " + mLibs[g_idCurrentLib].name)
    );

    setMinimumSize(800, 400);
    if (settings.contains("MainWnd/geometry"))
        restoreGeometry(settings.value("MainWnd/geometry").toByteArray());
    if (settings.contains("MainWnd/windowState"))
        restoreState(settings.value("MainWnd/windowState").toByteArray());
    if (settings.contains("MainWnd/tab/geometry"))
        ui->splitter->restoreState(settings.value("MainWnd/tab/geometry").toByteArray());
    //on_splitter_splitterMoved(0,0);
    if (settings.contains("MainWnd/books/geometry"))
        ui->splitter_2->restoreState(settings.value("MainWnd/books/geometry").toByteArray());
    settings.setValue("ApplicationMode", mode);
}

/*
    загрузка списков Авторов, Серий, Жанров, книг, соответствующих выбранному языку в выпадающем списке языков на панели инструментов
*/
void MainWindow::on_comboBoxLanguageFilter_currentIndexChanged(const QString &arg1)
{
    // сохранение языка фильтрации книг текущей библиотеки с id = g_idCurrentLib
    SaveCurrentBookLanguageFilter(g_idCurrentLib, arg1);
    idCurrentLanguage_ = ui->comboBoxLanguageFilter->currentData().toInt();

    FillListWidgetAuthors(g_idCurrentLib);
    FillListWidgetSerials(g_idCurrentLib);
    FillTreeWidgetGenres(g_idCurrentLib);
    FillListBooks();
}

/*
    обработчик переключения в режим библиотеки из конвертера по нажатию кнопки
*/
void MainWindow::on_btnSwitchToLib_clicked()
{
    on_actionSwitch_to_library_mode_triggered();
}

/*
    обработчик вызова диалога настроек из конвертера по нажатию кнопки
*/
void MainWindow::on_btnPreference_clicked()
{
    Settings();
}

void MainWindow::resizeEvent(QResizeEvent */*e*/)
{

    if(pDropForm_!=nullptr)
    {
        if(pDropForm_->isVisible())
        {
            const QSignalBlocker blocker(this);
            pDropForm_->deleteLater();
            pDropForm_=nullptr;
            ShowDropForm();
        }
    }
}
void MainWindow::mouseMoveEvent(QMouseEvent *ev)
{
    if(mode==MODE_CONVERTER)
    {
        if(pDropForm_==nullptr)
        {
            ShowDropForm();
        }
        pDropForm_->switch_command(ev->pos());
    }
}

void MainWindow::leaveEvent(QEvent */*ev*/)
{
    if(pDropForm_!=nullptr)
    {
        pDropForm_->switch_command(QPoint(-1,-1));
    }
}

void MainWindow::mouseReleaseEvent(QMouseEvent *ev)
{
    if(mode==MODE_CONVERTER)
    {
        if(pDropForm_==nullptr)
        {
            ShowDropForm();
        }
        int id=pDropForm_->get_command(ev->pos());
        if(id<0)
            return;
        QFileDialog dialog(this);
        dialog.setFileMode(QFileDialog::ExistingFiles);
        dialog.setNameFilter(tr("Books")+" (*.fb2 *.epub *.zip)");
        dialog.setWindowTitle(tr("Book`s files"));
        if (dialog.exec())
        {
            if(dialog.selectedFiles().isEmpty())
                return;
            QStringList book_list;
            foreach (QString file, dialog.selectedFiles())
            {
                ProcPath(file,&book_list);
            }
            ExportDlg dlg(this);
            dlg.exec(book_list,SetCurrentExportSettings(id));
        }
    }
}

void MainWindow::ChangingTrayIcon(int index,int color)
{
    if(CMDparser.isSet("tray"))
        index=2;
    QSettings settings;
    if(index<0)
    {
        index=settings.value("tray_icon",0).toInt();
    }
    if(color<0)
    {
        color=settings.value("tray_color",0).toInt();
    }
    if(index==0)
    {
        if(trayIcon_)
        {
            trayIcon_->hide();
            trayIcon_->deleteLater();
        }
        trayIcon_=nullptr;
    }
    else
    {
        if(!trayIcon_)
        {
            trayIcon_ = new QSystemTrayIcon(this);  //инициализируем объект
            connect(trayIcon_,SIGNAL(activated(QSystemTrayIcon::ActivationReason)),this,SLOT(TrayMenuAction(QSystemTrayIcon::ActivationReason)));
        }
        QIcon icon(QString(":/img/tray%1.png").arg(QString::number(color)));
        trayIcon_->setIcon(icon);//.arg(app->devicePixelRatio()>=2?"@2x":"")));  //устанавливаем иконку
        trayIcon_->show();
    }
}

void MainWindow::TrayMenuAction(QSystemTrayIcon::ActivationReason reson)
{
    if(reson!=QSystemTrayIcon::Trigger && reson!=QSystemTrayIcon::Unknown)
        return;
    QSettings settings;
#ifdef Q_OS_WIN
    if(this->isVisible())
    {
        this->setWindowState(this->windowState()|Qt::WindowMinimized);
        if(settings.value("tray_icon",0).toInt()!=0)
            this->hide();
    }
    else
    {
        this->show();
        this->setWindowState(this->windowState() & ~Qt::WindowMinimized);
        this->raise();
        this->activateWindow();
        this->setFocus(Qt::ActiveWindowFocusReason);
    }
#else
    #ifdef Q_OS_OSX
        if(reson==QSystemTrayIcon::Unknown)
            return;
        if(this->isActiveWindow() && this->isVisible())
        {
            this->setWindowState(this->windowState()|Qt::WindowMinimized);
            if(settings.value("tray_icon",0).toInt()!=0)
                this->hide();
        }
        else
        {
            this->show();
            this->setWindowState(this->windowState() & ~Qt::WindowMinimized);
            this->activateWindow();
            this->raise();
            this->setFocus(Qt::ActiveWindowFocusReason);
        }
    #else
        if(this->isActiveWindow() && this->isVisible())
        {
            this->setWindowState(this->windowState()|Qt::WindowMinimized);
            if(settings.value("tray_icon",0).toInt()!=0)
                this->hide();
        }
        else
        {
            this->show();
            this->setWindowState(this->windowState() & ~Qt::WindowMinimized);
            this->raise();
            this->activateWindow();
            this->setFocus(Qt::ActiveWindowFocusReason);
        }
    #endif
#endif
}

void MainWindow::dockClicked()
{
    //qDebug()<<"dock";
}

void MainWindow::MinimizeWindow()
{
    this->setWindowState(this->windowState()|Qt::WindowMinimized);
}

void MainWindow::changeEvent(QEvent *event)
{
    if(event->type() == QEvent::WindowStateChange)
    {
        if(isMinimized())
        {
            ChangingTrayIcon();
            TrayMenuAction(QSystemTrayIcon::Unknown);
            event->ignore();
        }
    }
}

/*
    чтение из базы данные 'позиции' библиотеки
*/
int MainWindow::LoadLibraryPosition(uint idLibrary)
{
    // чтение из базы 'позиции' для текущей библиотеки с id = idLibrary
    QSqlQuery query(QSqlDatabase::database("libdb"));
    query.setForwardOnly(true);
    query.prepare(
        "SELECT currentTab, currentAuthor, currentSeria, currentGenre, currentGroup, currentBookForAuthor, currentBookForSeria, currentBookForGenre, currentBookForGroup, currentSearchingFilter, currentTag, currentBookLanguage FROM lib WHERE id=:id;"
    );
    //              0           1                2            3              4                  5                   6                   7                   8                      9                 10             11
    query.bindValue(":id", idLibrary);
    if (!query.exec())
        qDebug() << query.lastError().text();
    query.next();
    int nCurrentTab = query.value(0).toInt();
    mLibs[idLibrary].uIdCurrentAuthor = query.value(1).toUInt();
    mLibs[idLibrary].uIdCurrentSeria = query.value(2).toUInt();
    mLibs[idLibrary].uIdCurrentGenre = query.value(3).toUInt();
    mLibs[idLibrary].uIdCurrentGroup = query.value(4).toUInt();
    mLibs[idLibrary].uIdCurrentBookForAuthor = query.value(5).toUInt();
    mLibs[idLibrary].uIdCurrentBookForSeria = query.value(6).toUInt();
    mLibs[idLibrary].uIdCurrentBookForGenre = query.value(7).toUInt();
    mLibs[idLibrary].uIdCurrentBookForGroup = query.value(8).toUInt();
    mLibs[idLibrary].sCurrentSearchingFilter = query.value(9).toString();
    ui->lineEditSearchString->setText(query.value(9).toString());
    mLibs[idLibrary].uIdCurrentTag = query.value(10).toUInt();
    mLibs[idLibrary].sCurrentBookLanguage = query.value(11).toString();
    return nCurrentTab;
}

/*
    сохранение языка фильтрации книг текущей библиотеки с id = idLibrary
*/
void MainWindow::SaveCurrentBookLanguageFilter(uint idLibrary, const QString& lang)
{
    QSqlQuery query(QSqlDatabase::database("libdb"));
    query.prepare("UPDATE lib SET currentBookLanguage = :currentBookLanguage WHERE id = :id_lib;");
    query.bindValue(":currentBookLanguage", lang);
    query.bindValue(":id_lib", idLibrary);
    query.exec();
}

/*
    заполнение комбобокса рейтинга на вкладке Поиска
*/
void MainWindow::FillRatingList()
{
    ui->comboBoxFindRating->addItem("*", -1);
    for (int i = 0; i < 6; i++)
        ui->comboBoxFindRating->addItem(QString("%1").arg(i), i);
}

/*
    пометка ячейки статуса 'Прочитано'
*/
void MainWindow::MarkReadedBook(QTreeWidgetItem* bookItem, bool idReaded)
{
    bookItem->setText(8, idReaded == 1 ? tr("Yes") : "");
    //bookItem->setBackgroundColor(8, idReaded == 1 ? QColor(0, 255, 0) : QColor(255, 255, 255));
    bookItem->setIcon(8, idReaded == 1 ? QIcon(":/icons/img/icons/Streamline.png") : QIcon());
}

/*
    обработчик кнопки отображения Групп книг
*/
void MainWindow::btnPageGroupsClick()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    ui->tabWidget->setCurrentIndex(4);
    ui->SearchFrame->setEnabled(false);
    ui->ExpandTreeGemresFrame->setVisible(false);
    ui->frameLang->setEnabled(false);
    ui->Books->clear();
    ui->comboBoxLanguageFilter->setEnabled(true);
    ui->comboBoxTagFilter->setEnabled(true);
    SelectGroup();
    QApplication::restoreOverrideCursor();
}

/*
    обработчик сигнала выделения/снятия выделения итема списка книг Группы
*/
void MainWindow::SelectionChangedGroupsList(const QItemSelection& selected, const QItemSelection& /*deselected*/)
{
    // установка доступности/недоступности контролов, в зависимости от числа итемов виджета списка Групп
    SetEnabledOrDisabledControllsOfSelectedStateItemGroups(selected);
}

/*
    установка доступности / недоступности контролов, в зависимости от числа итемов виджета списка Групп
*/
void MainWindow::SetEnabledOrDisabledControllsOfSelectedStateItemGroups(const QItemSelection& selected)
{
    if (ui->GroupList->selectedItems().count() > 0) {
        QModelIndex index = selected.indexes()[0];
        uint idGroup = ui->GroupList->item(index.row())->data(Qt::UserRole).toUInt();
        if (mLibs[g_idCurrentLib].mGroups[idGroup].isBlocked()) {
            // для 3-х заблокированных от удаления/переименования Групп
            ui->btnGroupRemove->setDisabled(true);
            ui->btnGroupRename->setDisabled(true);
        }
        else {
            // для всех остальных, незаблокированных групп
            ui->btnGroupRemove->setEnabled(true);
            ui->btnGroupRename->setEnabled(true);
        }
    }
}

/*
    обработчик кнопки добавления Группы в список Групп
*/
void MainWindow::AddGroupToList()
{
    bool ok;
    QString GroupName = QInputDialog::getText(
        this, tr("Input Group"), tr("New Group:"), QLineEdit::Normal, tr("New Group"), &ok
    );
    GroupName = GroupName.trimmed();
    if (ok && !GroupName.isEmpty()) {
        QSqlQuery query(QSqlDatabase::database("libdb"));
        // проверка на наличие в списке добавляемой группы
        query.prepare("SELECT name FROM groups WHERE id_lib = :id_lib;");
        query.bindValue(":id_lib", g_idCurrentLib);
        if (!query.exec())
            qDebug() << query.lastError().text();
        while (query.next()) {
            if (GroupName == query.value(0).toString().trimmed()) {
                QMessageBox::warning(this, tr("Add new group"), tr("This group is already on the group list!"), QMessageBox::Ok);
                return;
            }
        }
        // вставка новой группы в базу данных
        qlonglong id = -1;
        query.prepare("INSERT INTO groups(name, id_lib) values(:name, :id_lib);");
        query.bindValue(":name", GroupName);
        query.bindValue(":id_lib", g_idCurrentLib);
        if (!query.exec())
            qDebug() << query.lastError().text();
        else {
            id = query.lastInsertId().toLongLong();
            QListWidgetItem* item;
            item = new QListWidgetItem(GroupName);
            item->setData(Qt::UserRole, id);
            ui->GroupList->insertItem(ui->GroupList->count(), item);
            // добавление новой группы в структуру текущей библиотеки
            Group newGroup(GroupName, id, false);
            mLibs[g_idCurrentLib].mGroups.insert(id, newGroup);
        }
    }
}

/*
    добавление выделенной книги в Группу
*/
void MainWindow::AddBookToGroupAction()
{
    QTreeWidgetItem* bookItem = ui->Books->selectedItems()[0];
    if (bookItem->type() == ITEM_TYPE_BOOK) {
        uint group_id = qobject_cast<QAction*>(QObject::sender())->data().toInt();
        uint book_id = bookItem->data(0, Qt::UserRole).toUInt();
        QSqlQuery query(QSqlDatabase::database("libdb"));
        // проверка, если ли добавляемая книга в группе, и выдача сообщения, если есть
        query.prepare("SELECT book_id FROM book_group WHERE id_lib = :id_lib AND group_id = :group_id;");
        query.bindValue(":group_id", group_id);
        query.bindValue(":id_lib", g_idCurrentLib);
        if (!query.exec())
            qDebug() << query.lastError().text();
        while (query.next()) {
            if (book_id == query.value(0).toUInt()) {
                QMessageBox::warning(this, tr("Adding a book to the group"), tr("This book has already been previously added to the group!"), QMessageBox::Ok);
                return;
            }
        }
        // добавление выделенной книги в базу и структуру текущей библиотеки
        query.prepare("INSERT INTO book_group(book_id, group_id, id_lib) values(:book_id, :group_id, :id_lib);");
        query.bindValue(":book_id", book_id);
        query.bindValue(":group_id", group_id);
        query.bindValue(":id_lib", g_idCurrentLib);
        if (!query.exec())
            qDebug() << query.lastError().text();
        else {
            mLibs[g_idCurrentLib].mGroupBooksLink.insert(group_id, book_id);
            // изменение отображения в названии группы числа книг
            SetNewGroupNameWithBookCount(g_idCurrentLib, group_id);
        }
    }
}

/*
    изменение названия группы
*/
void MainWindow::RenameGroup()
{
    if (ui->GroupList->selectedItems().count() > 0) {
        // название Группы без числа книг в ней
        QString oldGroupNameWithoutBookCount = GetGroupNameWhitoutBookCount(g_idCurrentLib, mLibs[g_idCurrentLib].uIdCurrentGroup);
        bool ok;
        QString newGroupName = QInputDialog::getText(
            this, tr("Input Group"), tr("New name Group:"), QLineEdit::Normal, oldGroupNameWithoutBookCount, &ok
        );
        newGroupName = newGroupName.trimmed();
        if (ok && !newGroupName.isEmpty() && newGroupName != oldGroupNameWithoutBookCount) {
            QSqlQuery query(QSqlDatabase::database("libdb"));
            // проверка на наличие в списке добавляемой группы
            query.prepare("SELECT name FROM groups WHERE id_lib = :id_lib;");
            query.bindValue(":id_lib", g_idCurrentLib);
            if (!query.exec())
                qDebug() << query.lastError().text();
            while (query.next()) {
                if (newGroupName == query.value(0).toString().trimmed()) {
                    QMessageBox::warning(this, tr("Add new group"), tr("This group is already on the group list!"), QMessageBox::Ok);
                    return;
                }
            }
            // изменение названия группы в базе данных
            query.prepare("UPDATE groups SET name = :name WHERE id_lib = :id_lib AND id = :id;");
            query.bindValue(":id_lib", g_idCurrentLib);
            query.bindValue(":id", mLibs[g_idCurrentLib].uIdCurrentGroup);
            query.bindValue(":name", newGroupName);
            if (!query.exec())
                qDebug() << query.lastError().text();
            else {
                // изменение названия группы в структуре библиотеки
                QHash<uint, Group>::iterator GroupIterator = mLibs[g_idCurrentLib].mGroups.begin();
                while (GroupIterator != mLibs[g_idCurrentLib].mGroups.end()) {
                    if (GroupIterator.value().getName() == oldGroupNameWithoutBookCount) {
                        GroupIterator.value().setName(newGroupName);
                        break;
                    }
                    ++GroupIterator;
                }
                // изменение названия группы в контроле списка групп
                uint bookCount = GetBookCountFromGroup(g_idCurrentLib, mLibs[g_idCurrentLib].uIdCurrentGroup);
                QString itemName;
                itemName = bookCount > 0
                    ? QString("%1 (%2)").arg(newGroupName).arg(GetBookCountFromGroup(g_idCurrentLib, mLibs[g_idCurrentLib].uIdCurrentGroup))
                    : newGroupName;
                QListWidgetItem* selectedItem = ui->GroupList->selectedItems()[0];
                selectedItem->setText(itemName);
            }
        }
    }
}

/*
    удаление выделенной книги из выделенной группы
*/
void MainWindow::DeleteBookFromGroupAction()
{
    QTreeWidgetItem* bookItem = ui->Books->selectedItems()[0];
    if (QMessageBox::question(
        this, tr("Remove book from group"),
        tr("Are you sure you want to delete from the group of the book") + "\n'" + bookItem->text(0) + "'?",
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes) {

        uint idBook = bookItem->data(0, Qt::UserRole).toUInt();

        // удаление в базе книг из сформированного списка
        QSqlQuery query(QSqlDatabase::database("libdb"));
        query.prepare("DELETE FROM book_group WHERE id_lib = :id_lib AND group_id = :group_id AND book_id = :book_id;");
        query.bindValue(":book_id", idBook);
        query.bindValue(":group_id", mLibs[g_idCurrentLib].uIdCurrentGroup);
        query.bindValue(":id_lib", g_idCurrentLib);
        if (!query.exec())
            qDebug() << query.lastError().text();

        // удаление из структуры связи этой книги с выделенной группой
        mLibs[g_idCurrentLib].mGroupBooksLink.remove(mLibs[g_idCurrentLib].uIdCurrentGroup, idBook);

        // изменение отображения в названии группы числа книг
        SetNewGroupNameWithBookCount(g_idCurrentLib, mLibs[g_idCurrentLib].uIdCurrentGroup);

        // удаление книги из контрола дерева книг
        SelectGroup();
    }
}
/*
    удаление всех книг из выделенной группы
*/
void MainWindow::DeleteAllBooksFromGroup()
{
    if (ui->GroupList->selectedItems().count() > 0) {
        // число книг в группе
        int bookCount = GetBookCountFromGroup(g_idCurrentLib, mLibs[g_idCurrentLib].uIdCurrentGroup);
        if (bookCount > 0) {
            QString selectedGroupName = ui->GroupList->selectedItems()[0]->text();
            if (QMessageBox::question(
                this, tr("Clear selected group"),
                tr("Are you sure you want to delete all books of the selected group") + " '" + selectedGroupName + "'?",
                QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes) {
                // удаление всех книг из выделенной группы
                RemoveAllBooksFromGroup(g_idCurrentLib, mLibs[g_idCurrentLib].uIdCurrentGroup);
                // корректировка числа книг в названии Группы
                ui->GroupList->selectedItems()[0]->setText(GetGroupNameWhitoutBookCount(g_idCurrentLib, mLibs[g_idCurrentLib].uIdCurrentGroup));
            }
        }
    }
}

/*
    удаление группы из списка групп
*/
void MainWindow::RemoveGroupFromList()
{
    if (ui->GroupList->selectedItems().count() > 0) {
        QString selectedGroupName = ui->GroupList->selectedItems()[0]->text();
        if (QMessageBox::question(
            this, tr("Remove selected group"),
            tr("Are you sure you want to delete the group") + " '" + selectedGroupName + "'?",
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes) {

            // удаление всех книг из выделенной группы
            RemoveAllBooksFromGroup(g_idCurrentLib, mLibs[g_idCurrentLib].uIdCurrentGroup);

            // Удаление выбранной группы из базы
            QSqlQuery query(QSqlDatabase::database("libdb"));
            query.prepare("DELETE FROM groups WHERE id_lib = :id_lib AND id = :group_id;");
            query.bindValue(":group_id", mLibs[g_idCurrentLib].uIdCurrentGroup);
            query.bindValue(":id_lib", g_idCurrentLib);
            if (!query.exec())
                qDebug() << query.lastError().text();

            // Удаление выбранной группы из структуры текущей библиотеки
            for (QHash<uint, Group>::iterator it = mLibs[g_idCurrentLib].mGroups.begin(); it != mLibs[g_idCurrentLib].mGroups.end(); ++it) {
                if ((*it).getId() == mLibs[g_idCurrentLib].uIdCurrentGroup) {
                    mLibs[g_idCurrentLib].mGroups.erase(it);
                    break;
                }
            }

            // Удаление выбранной группы из списка групп
            delete ui->GroupList->takeItem(ui->GroupList->currentRow());
        }
    }
}

/*
    удаление всех книг из выделенной группы
*/
void MainWindow::RemoveAllBooksFromGroup(uint idLibrary, uint idGroup)
{
    // Список книг для выделенной Группы
    QList<uint> listBooks = mLibs[idLibrary].mGroupBooksLink.values(idGroup);

    // удаление в базе книг из сформированного списка
    QSqlQuery query(QSqlDatabase::database("libdb"));
    foreach (uint book_id, listBooks) {
        query.prepare("DELETE FROM book_group WHERE id_lib = :id_lib AND group_id = :group_id AND book_id = :book_id;");
        query.bindValue(":book_id", book_id);
        query.bindValue(":group_id", idGroup);
        query.bindValue(":id_lib", idLibrary);
        if (!query.exec())
            qDebug() << query.lastError().text();
    }

    // удаление из структуры связи этой книги с выделенной группой
    QList<uint> booksIdList = mLibs[idLibrary].mGroupBooksLink.values(idGroup);
    foreach (uint bookId, booksIdList) {
        QMutableHashIterator<uint, uint> iter(mLibs[idLibrary].mGroupBooksLink);
        while (iter.hasNext()) {
            iter.next();
            if (idGroup == iter.key() && bookId == iter.value()) {
                iter.remove();
                break;
            }
        }
    }

    ui->Books->clear();
}

/*
    заполнение комбобокса с форматами книг на вкладке Поиск
*/
void MainWindow::FillFormatList(uint idLibrary)
{
    QSqlQuery query(QSqlDatabase::database("libdb"));
    query.prepare("SELECT format FROM book WHERE id_lib = :id_lib GROUP BY format;");
    query.bindValue(":id_lib", idLibrary);
    if (!query.exec())
        qDebug() << query.lastError().text();
    else {
        ui->comboBoxFindFormat->clear();
        ui->comboBoxFindFormat->addItem("*", Qt::UserRole);
        while (query.next())
            ui->comboBoxFindFormat->addItem(query.value(0).toString(), Qt::UserRole);
    }
}

/*
    создание меню Рейтинга
*/
void MainWindow::CreateRatingMenu()
{
    menuRating_ = new QMenu(tr("Rating"), this);
    for (int i = 0; i < 6; i++) {
        QAction* actionStar = new QAction(QString("%1").arg(i), this);
        actionStar->setData(QString::number(i).toInt());
        actionStar->setShortcut("Alt+" + QString::number(i));
        this->addAction(actionStar); // для срабатывания шортката
        connect(actionStar, &QAction::triggered, this, &MainWindow::RatingAction);
        menuRating_->addAction(actionStar);
    }
}

/*
    создание меню Прочитано/Не прочитано
*/
void MainWindow::CreateReadedMenu()
{
    menuReaded_ = new QMenu(tr("Readed"), this);
    QAction* actionReaded = new QAction(tr("Readed"), this);
    actionReaded->setData(QString::number(1).toInt());
    connect(actionReaded, &QAction::triggered, this, &MainWindow::ReadedAction);
    menuReaded_->addAction(actionReaded);
    actionReaded = new QAction(tr("Not readed"), this);
    actionReaded->setData(QString::number(0).toInt());
    connect(actionReaded, &QAction::triggered, this, &MainWindow::ReadedAction);
    menuReaded_->addAction(actionReaded);
}

/*
    Оптимизация базы данных
*/
void MainWindow::DatabaseOptimization()
{
    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    QSqlQuery query(QSqlDatabase::database("libdb"));
    query.exec("VACUUM");
    QApplication::restoreOverrideCursor();
    QMessageBox::information(this, tr("Database optimization"), tr("Database optimization completed."));
}

/*
    переименование названия Группы с учетом числа книг в ней
*/
void MainWindow::SetNewGroupNameWithBookCount(uint idLibrary, uint idGroup)
{
    // название Группы без числа книг в ней
    QString GroupName = mLibs[idLibrary].mGroups.find(idGroup).value().getName();
    int count = GetBookCountFromGroup(idLibrary, idGroup);

    if (count >= 1)
        GroupName += QString(" (%1)").arg(count);

    QListWidgetItem* item = nullptr;
    for (int i = 0; i < ui->GroupList->count(); ++i) {
        item = ui->GroupList->item(i);
        if (item->data(Qt::UserRole).toUInt() == idGroup) {
            item->setText(GroupName);
            break;
        }
    }
}

/*
    число книг в группе
*/
int MainWindow::GetBookCountFromGroup(uint idLibrary, uint idGroup)
{
    return mLibs[idLibrary].mGroupBooksLink.values(idGroup).count();
}

/*
    название Группы без числа книг в ней
*/
QString MainWindow::GetGroupNameWhitoutBookCount(uint idLibrary, uint idGroup)
{
    return mLibs[idLibrary].mGroups.find(idGroup).value().getName();
}

/*
    создание и вызов контекстного меню для списка Групп
*/
void MainWindow::GroupContextMenu(QPoint point)
{
    if (QObject::sender() == qobject_cast<QObject*>(ui->GroupList) && !ui->GroupList->itemAt(point))
        return;

    currentListForTag_ = QObject::sender();
    QMenu menu;
    if (ui->GroupList->selectedItems().count() > 0) {
        QListWidgetItem* item = ui->GroupList->selectedItems()[0];
        uint idGroup = item->data(Qt::UserRole).toUInt();
        if (!item->icon().isNull()) {
            QAction* actionDeleteIcon = new QAction(tr("Remove the Group icon..."), this);
            actionDeleteIcon->setData(QString::number(idGroup).toUInt());
            connect(actionDeleteIcon, &QAction::triggered, this, &MainWindow::DeleteGroupIconAction);
            menu.addAction(actionDeleteIcon);
        }

        QAction* actionSetIcon = new QAction(tr("Set the Group icon..."), this);
        actionSetIcon->setData(QString::number(idGroup).toUInt());
        connect(actionSetIcon, &QAction::triggered, this, &MainWindow::SetGroupIconAction);
        menu.addAction(actionSetIcon);
        menu.addSeparator();

        QAction* actionSetDefaultIcon = new QAction(tr("Default icons..."), this);
        actionSetDefaultIcon->setData(QString::number(idGroup).toUInt());
        connect(actionSetDefaultIcon, &QAction::triggered, this, &MainWindow::SetGroupDefaultIconsAction);
        menu.addAction(actionSetDefaultIcon);

        if (menu.actions().count() > 0)
            menu.exec(QCursor::pos());
    }
}

/*
  обработчик контекстного меню Групп по удалению иконки выделенной Группы  
*/
void MainWindow::DeleteGroupIconAction()
{
    QListWidgetItem* item = ui->GroupList->selectedItems()[0];
    int idGroup = item->data(Qt::UserRole).toInt();
    if (QMessageBox::question(
        this, tr("Remove the Group icon"),
        tr("Are you sure you want to remove the icon for the selected group?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes) {
        QSqlQuery query(QSqlDatabase::database("libdb"));
        query.prepare("UPDATE groups SET icon = :icon WHERE id_lib = :id_lib AND id = :id_group;");
        query.bindValue(":id_lib", g_idCurrentLib);
        query.bindValue(":id_group", idGroup);
        query.bindValue(":icon", QByteArray());
        if (!query.exec())
            qDebug() << query.lastError().text();
        else
            item->setIcon(QIcon());
    }
}

/*
  обработчик контекстного меню Групп по присвоению иконки выделенной Группы
*/
void MainWindow::SetGroupIconAction()
{
    QString iconPath = QFileDialog::getOpenFileName(
        this, tr("Open Image"), QDir::homePath(), tr("Image Files (*.png *.jpg *.jpeg *.ico)")
    );
    if (!iconPath.isEmpty()) {
        QListWidgetItem* item = ui->GroupList->selectedItems()[0];
        int idGroup = item->data(Qt::UserRole).toInt();
        QSqlQuery query(QSqlDatabase::database("libdb"));
        QPixmap pixmap(iconPath);
        QByteArray byteArray;
        QBuffer buffer(&byteArray);
        buffer.open(QIODevice::WriteOnly);
        pixmap.save(&buffer, "PNG");
        query.prepare("UPDATE groups SET icon = :icon WHERE id_lib = :id_lib AND id = :id_group;");
        query.bindValue(":id_lib", g_idCurrentLib);
        query.bindValue(":id_group", idGroup);
        query.bindValue(":icon", byteArray);
        if (!query.exec())
            qDebug() << query.lastError().text();
        else
            item->setIcon(QIcon(pixmap));
    }
}

/*
    обработчик контекстного меню Групп по заданию иконок заблокированных Групп по умолчанию
*/
void MainWindow::SetGroupDefaultIconsAction()
{
    if (QMessageBox::question(
        this, tr("Default icons"),
        tr("Are you sure you want to set default icons for blocked Groups?"), QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::Yes) {
        QSqlQuery query(QSqlDatabase::database("libdb"));
        QHash<uint, Group> hGroups = mLibs[g_idCurrentLib].mGroups;
        for (int i = 0; i < ui->GroupList->count(); ++i) {
            QListWidgetItem* item = ui->GroupList->item(i);
            uint idGroup = item->data(Qt::UserRole).toUInt();
            QHash<uint, Group>::const_iterator ciGroup = hGroups.find(idGroup);
            QString blockedName = ciGroup.value().getBlockedName();
            if (blockedName == "favorites") {
                // Избранное
                QPixmap favoritesPixmap(":/icons/img/icons/favorites.png");
                QByteArray favoritesByteArray;
                QBuffer favoritesBuffer(&favoritesByteArray);
                favoritesBuffer.open(QIODevice::WriteOnly);
                favoritesPixmap.save(&favoritesBuffer, "PNG");
                query.prepare("UPDATE groups SET icon = :icon WHERE id_lib = :id_lib AND id = :id_group;");
                query.bindValue(":id_lib", g_idCurrentLib);
                query.bindValue(":id_group", idGroup);
                query.bindValue(":icon", favoritesByteArray);
                if (!query.exec())
                    qDebug() << query.lastError().text();
                else
                    item->setIcon(QIcon(favoritesPixmap));
            }
            else if (blockedName == "toRead") {
                // К прочтению
                QPixmap toReadPixmap(":/icons/img/icons/toRead.png");
                QByteArray toReadByteArray;
                QBuffer toReadBuffer(&toReadByteArray);
                toReadBuffer.open(QIODevice::WriteOnly);
                toReadPixmap.save(&toReadBuffer, "PNG");
                query.prepare("UPDATE groups SET icon = :icon WHERE id_lib = :id_lib AND id = :id_group;");
                query.bindValue(":id_lib", g_idCurrentLib);
                query.bindValue(":id_group", idGroup);
                query.bindValue(":icon", toReadByteArray);
                if (!query.exec())
                    qDebug() << query.lastError().text();
                else
                    item->setIcon(QIcon(toReadPixmap));
            }
            else if (blockedName == "read") {
                // Читаю
                QPixmap readPixmap(":/icons/img/icons/read.png");
                QByteArray readByteArray;
                QBuffer readBuffer(&readByteArray);
                readBuffer.open(QIODevice::WriteOnly);
                readPixmap.save(&readBuffer, "PNG");
                query.prepare("UPDATE groups SET icon = :icon WHERE id_lib = :id_lib AND id = :id_group;");
                query.bindValue(":id_lib", g_idCurrentLib);
                query.bindValue(":id_group", idGroup);
                query.bindValue(":icon", readByteArray);
                if (!query.exec())
                    qDebug() << query.lastError().text();
                else
                    item->setIcon(QIcon(readPixmap));
            }
        }
    }
}

void MainWindow::actionAboutQt()
{
    QMessageBox::aboutQt(this);
}
