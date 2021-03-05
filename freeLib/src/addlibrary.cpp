#include <QMainWindow>
#include <QToolButton>
#include <QInputDialog>

#include "addlibrary.h"
#include "ui_addlibrary.h"
#include "common.h"
#include "quazip/quazip/quazip.h"
#include "quazip/quazip/quazipfile.h"
#include "exportdlg.h"

AddLibrary::AddLibrary(QWidget *parent) :
    QDialog(parent,Qt::Dialog|Qt::WindowSystemMenuHint),
    ui(new Ui::AddLibrary)
{
    bLibChanged_ = false;
    ui->setupUi(this);

    QToolButton* tbInpx=new QToolButton(this);
    tbInpx->setFocusPolicy(Qt::NoFocus);
    tbInpx->setCursor(Qt::ArrowCursor);
    tbInpx->setText("...");
    QHBoxLayout* layout=new QHBoxLayout(ui->lineEditInpxFile);
    layout->addWidget(tbInpx,0,Qt::AlignRight);
    layout->setSpacing(0);
    layout->setMargin(0);

    QToolButton* tbBooksDir=new QToolButton(this);
    tbBooksDir->setFocusPolicy(Qt::NoFocus);
    tbBooksDir->setCursor(Qt::ArrowCursor);
    tbBooksDir->setText("...");
    layout=new QHBoxLayout(ui->lineEditBooksDir);
    layout->addWidget(tbBooksDir,0,Qt::AlignRight);
    layout->setSpacing(0);
    layout->setMargin(0);

    idCurrentLib_ = g_idCurrentLib;
    // установка контролов в состояние по-умолчанию, когда нет ни одной библиотеки
    SetControllsToDefaultState();
    UpdateLibList();

    connect(tbInpx,SIGNAL(clicked()),this,SLOT(InputINPX()));
    connect(tbBooksDir,SIGNAL(clicked()),this,SLOT(SelectBooksDir()));
    connect(ui->btnUpdateLibrary,SIGNAL(clicked()),this,SLOT(StartImport()));
    connect(ui->btnExportLibrary,SIGNAL(clicked()),this,SLOT(ExportLib()));
    connect(ui->comboBoxExistingLibs,SIGNAL(currentIndexChanged(int)),this,SLOT(SelectLibrary()));
    connect(ui->btnLibraryAdd, SIGNAL(clicked()), this, SLOT(AddNewLibrary()));
    connect(ui->btnLibraryEdit, &QToolButton::clicked, this, &AddLibrary::EditLibraryName);
    connect(ui->btnLibraryDelete, SIGNAL(clicked()), this, SLOT(DeleteLibrary()));
    connect(ui->comboBoxExistingLibs->lineEdit(),SIGNAL(editingFinished()),this,SLOT(ExistingLibsChanged()));
    connect(ui->btnSaveLog, &QPushButton::clicked, this, &AddLibrary::ButtonSaveLogClicked);
    connect(ui->btnBooksDirAdd, &QToolButton::clicked, this, &AddLibrary::AddBooksDirToList);
    connect(ui->btnBooksDirDelete, &QToolButton::clicked, this, &AddLibrary::DeleteDirFromBookDirsList);
    connect(ui->listWidgetBooksDirs->selectionModel(), &QItemSelectionModel::selectionChanged, this, &AddLibrary::SelectionChangedBookDirsList);
    connect(ui->lineEditBooksDir, &QLineEdit::textChanged, this, &AddLibrary::LineEditBooksDirTextChanged);
    connect(ui->checkBoxShowLog, &QCheckBox::clicked, this, &AddLibrary::ExpandLog);

    ui->rbtnAddNewBook->setChecked(true);

    SelectLibrary(idCurrentLib_);
//    SelectLibrary();
    // установка доступности/недоступности контролов, в зависимости от наличия выделения итемов виджета списка папок
    SetEnabledOrDisabledControllsOfSelectedStateItemBooksDirs();
    // установка доступности/недоступности контролов, в зависимости от числа итемов виджета списка папок
    SetEnabledOrDisabledControllsOfBooksDirs();
}

AddLibrary::~AddLibrary()
{
    delete ui;
}

bool AddLibrary::IsLibraryChanged() const
{
    return bLibChanged_;
}

void AddLibrary::AddNewLibrary()
{
    ui->Log->clear();
    idCurrentLib_ = -1;
    QString newLibraryName = tr("New Library") + " ("+ QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss") + ")";
    bool ok;
    QString editedLibraryName = QInputDialog::getText(
        this, tr("Input name"), tr("Library name:"), QLineEdit::Normal, newLibraryName, &ok
    );
    editedLibraryName = editedLibraryName.trimmed();
    if (ok && !editedLibraryName.isEmpty()) {
        newLibraryName = editedLibraryName;
        // Есть ли проверяемая библиотека в списке библиотек?
        if (IsLibraryWithNameExists(newLibraryName, tr("New library")))
            return;
        ui->comboBoxExistingLibs->blockSignals(true);
        ui->comboBoxExistingLibs->addItem(newLibraryName, -1);
        ui->comboBoxExistingLibs->setCurrentIndex(ui->comboBoxExistingLibs->count() - 1);
        // установка контролов в состояние по-умолчанию, когда нет ни одной библиотеки
        SetControllsToDefaultState();
        SLib lib;
        lib.name = newLibraryName;
        lib.bFirstAuthor = false;
        lib.bWoDeleted = false;
        SaveLibrary(idCurrentLib_, lib);
        ui->comboBoxExistingLibs->blockSignals(false);
        // установка доступности/недоступности контролов, в зависимости от числа итемов виджета списка папок
        SetEnabledOrDisabledControllsOfBooksDirs();
        ui->btnSaveLog->setEnabled(ui->Log->count() > 1);
    }
}

/*
    правка названия библиотеки
*/
void AddLibrary::EditLibraryName()
{
    if (ui->comboBoxExistingLibs->currentIndex() > -1) {
        bool ok;
        QString newLibraryName = QInputDialog::getText(
            this, tr("Input name"), tr("Library name:"), QLineEdit::Normal, ui->comboBoxExistingLibs->currentText(), &ok
        );
        newLibraryName = newLibraryName.trimmed();
        if (ok && !newLibraryName.isEmpty()) {
            // Есть ли проверяемая библиотека в списке библиотек?
            if (IsLibraryWithNameExists(newLibraryName, tr("Edit the name of the library")))
                return;
            ui->comboBoxExistingLibs->blockSignals(true);
            ui->comboBoxExistingLibs->setItemText(ui->comboBoxExistingLibs->currentIndex(), newLibraryName);
            ui->comboBoxExistingLibs->blockSignals(false);
            mLibs[idCurrentLib_].name = newLibraryName;
            SaveLibrary(idCurrentLib_, mLibs[idCurrentLib_]);
        }
    }
}

void AddLibrary::LogMessage(QString msg)
{
    while(ui->Log->count()>100)
        delete ui->Log->takeItem(0);
    ui->Log->addItem(msg);
    ui->Log->setCurrentRow(ui->Log->count()-1);
    LogList_ << msg;
}
void AddLibrary::InputINPX()
{
    QDir::setCurrent(QFileInfo(ui->lineEditInpxFile->text()).absolutePath());
    QString fileName = QFileDialog::getOpenFileName(this, tr("Add library"),"",tr("Library")+" (*.inpx)");
    if(!fileName.isEmpty())
    {
        ui->lineEditInpxFile->setText(fileName);
        ui->lineEditBooksDir->setText(QFileInfo(fileName).absolutePath());
        QuaZip uz(fileName);
        if(!uz.open(QuaZip::mdUnzip))
        {
            qDebug()<<"Error open INPX file: "<<fileName;
            return;
        }
        if(SetCurrentZipFileName(&uz,"COLLECTION.INFO"))
        {
            QBuffer outbuff;
            QuaZipFile zip_file(&uz);
            zip_file.open(QIODevice::ReadOnly);
            outbuff.setData(zip_file.readAll());
            zip_file.close();
            QString sLib = QString::fromUtf8(outbuff.data().left(outbuff.data().indexOf('\n')));
            ui->comboBoxExistingLibs->setItemText(ui->comboBoxExistingLibs->currentIndex(),sLib);
        }
    }
}
void AddLibrary::SelectBooksDir()
{
    QDir::setCurrent(ui->lineEditBooksDir->text());
    QString dir = QFileDialog::getExistingDirectory(this, tr("Select books directory"));
    if (!dir.isEmpty())
        ui->lineEditBooksDir->setText(dir);
}

void AddLibrary::UpdateLibList()

{
    if (!db_is_open)
        return;
    bool block = ui->comboBoxExistingLibs->blockSignals(true);
    ui->comboBoxExistingLibs->clear();
    QMap<int, SLib>::const_iterator i = mLibs.constBegin();
    while (i != mLibs.constEnd()) {
        if (i.key() != -1)
            ui->comboBoxExistingLibs->addItem(i->name, i.key());
        ++i;
    }
    ui->comboBoxExistingLibs->blockSignals(block);
}

void AddLibrary::StartImport()
{
    SLib lib;
    lib.name = ui->comboBoxExistingLibs->currentText().trimmed();
    lib.sInpx = ui->lineEditInpxFile->text().trimmed();
    lib.bFirstAuthor = ui->checkBoxFirstAuthorOnly->isChecked();
    lib.bWoDeleted = ui->checkBoxWoDeleted->isChecked();
    // формирование комбинированной строки с путями к каталогам с книгами
    QString DirsPath;
    for (int i = 0; i < ui->listWidgetBooksDirs->count(); ++i)
        DirsPath += ui->listWidgetBooksDirs->item(i)->text() + "|";
    if (DirsPath.right(1) == "|")
        DirsPath = DirsPath.remove(DirsPath.length() - 1, 1);
    lib.path = DirsPath;

    StartImport(lib);
}

void AddLibrary::StartImport(SLib &Lib)
{
    QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
    LogList_.clear();
    // UT_NEW: Добавить новые книги
    // UT_FULL: Пересоздать библиотеку
    // UT_DEL_AND_NEW : Удалить несуществующие и добавить новые книги
    int update_type=(ui->rbtnAddNewBook->isChecked()?UT_NEW:ui->rbtnDeleleOldBook->isChecked()?UT_DEL_AND_NEW:UT_FULL);
    SaveLibrary(idCurrentLib_,Lib);
    // занесение в таблицу groups две неудаляемые Группы
    AddGroupToSQLite(idCurrentLib_);

    ui->btnExportLibrary->setDisabled(true);
    ui->btnUpdateLibrary->setDisabled(true);
    ui->lineEditBooksDir->setDisabled(true);
    ui->lineEditInpxFile->setDisabled(true);
    ui->comboBoxExistingLibs->setDisabled(true);
    ui->btnLibraryAdd->setDisabled(true);
    ui->btnLibraryEdit->setDisabled(true);
    ui->btnLibraryDelete->setDisabled(true);
    ui->checkBoxFirstAuthorOnly->setDisabled(true);
    ui->checkBoxWoDeleted->setDisabled(true);
    ui->btnCancel->setText(tr("Break"));
    ui->checkBoxShowLog->setChecked(true);
    ExpandLog();

    thread_ = new QThread;
    imp_tr_=new ImportThread();
    imp_tr_->start(Lib.sInpx,Lib.name,Lib.path,idCurrentLib_,update_type,false,
                  Lib.bFirstAuthor&&Lib.sInpx.isEmpty(),Lib.bWoDeleted);
    imp_tr_->moveToThread(thread_);
    connect(imp_tr_, SIGNAL(Message(QString)), this, SLOT(LogMessage(QString)));
    connect(thread_, SIGNAL(started()), imp_tr_, SLOT(process()));
    connect(imp_tr_, SIGNAL(End()), thread_, SLOT(quit()));
    connect(thread_, SIGNAL(finished()), thread_, SLOT(deleteLater()));
    connect(imp_tr_, SIGNAL(End()), this, SLOT(EndUpdate()));
    connect(this, SIGNAL(break_import()), imp_tr_, SLOT(break_import()));

    thread_->start();
}

void AddLibrary::ImportNewLibrary(SLib &lib)
{
    if(!db_is_open)
    {
        db_is_open=openDB(true,false);
    }
    idCurrentLib_ =-1;
    StartImport(lib);
    exec();
}

// формирования списка каталогов с книгами для текущей библиотеки
void AddLibrary::MakeDirsList()
{
    ui->lineEditBooksDir->clear();
    ui->listWidgetBooksDirs->clear();
    QString DirsPath = mLibs[idCurrentLib_].path.trimmed();
    if (!DirsPath.isEmpty())
    {
        QStringList DirList = DirsPath.split("|");
        ui->listWidgetBooksDirs->addItems(DirList);
        ui->lineEditBooksDir->setText(DirList[0]);
    }
}

void AddLibrary::SelectLibrary(int idLib)
{
    bool block = ui->comboBoxExistingLibs->blockSignals(true);
    if(idLib>=0 && mLibs.count()>0){
        for(int i=0;i<ui->comboBoxExistingLibs->count();i++){
            if(ui->comboBoxExistingLibs->itemData(i).toInt()==idCurrentLib_){
                ui->comboBoxExistingLibs->setCurrentIndex(i);
                ui->lineEditInpxFile->setText(mLibs[idCurrentLib_].sInpx);
                ui->checkBoxFirstAuthorOnly->setChecked(mLibs[idCurrentLib_].bFirstAuthor);
                ui->checkBoxWoDeleted->setChecked(mLibs[idCurrentLib_].bWoDeleted);
                QSettings* settings=GetSettings();
                ui->labelOPDS->setText(idCurrentLib_<0?"":QString("<a href=\"http://localhost:%2/opds_%1\">http://localhost:%2/opds_%1</a>").arg(idCurrentLib_).arg(settings->value("OPDS_port",default_OPDS_port).toString()));
                ui->labelHTTP->setText(idCurrentLib_<0?"":QString("<a href=\"http://localhost:%2/http_%1\">http://localhost:%2/http_%1</a>").arg(idCurrentLib_).arg(settings->value("OPDS_port",default_OPDS_port).toString()));
                // формирования списка каталогов с книгами для текущей библиотеки
                MakeDirsList();
                break;
            }
        }
    }
    ui->btnLibraryDelete->setDisabled(idLib < 0);
    ui->btnLibraryEdit->setDisabled(idLib < 0);
    ui->comboBoxExistingLibs->setDisabled(idLib < 0);
    ui->lineEditInpxFile->setDisabled(idLib < 0);
    ui->lineEditBooksDir->setDisabled(idLib < 0);
    ui->btnExportLibrary->setDisabled(idLib < 0);
    ui->btnUpdateLibrary->setDisabled(idLib < 0);
    ui->listWidgetBooksDirs->setDisabled(idLib < 0);
    ui->comboBoxExistingLibs->blockSignals(block);
}

void AddLibrary::SelectLibrary()
{
    int nIndex = ui->comboBoxExistingLibs->currentIndex();
    QString inpx;
    bool firstAuthor = false;
    bool bWoDeleted = false;
    if (nIndex >= 0)
        idCurrentLib_ = ui->comboBoxExistingLibs->itemData(nIndex).toInt();
    if (idCurrentLib_ >= 0) {
        inpx = mLibs[idCurrentLib_].sInpx;
        firstAuthor = mLibs[idCurrentLib_].bFirstAuthor;
        bWoDeleted = mLibs[idCurrentLib_].bWoDeleted;
    }
    // формирования списка каталогов с книгами для текущей библиотеки
    MakeDirsList();
    ui->lineEditInpxFile->setText(inpx);
    ui->checkBoxFirstAuthorOnly->setChecked(firstAuthor);
    ui->checkBoxWoDeleted->setChecked(bWoDeleted);
    ui->btnLibraryDelete->setDisabled(idCurrentLib_ < 0);
    ui->btnLibraryEdit->setDisabled(idCurrentLib_ < 0);
    ui->comboBoxExistingLibs->setDisabled(idCurrentLib_ < 0);
    ui->lineEditInpxFile->setDisabled(idCurrentLib_ < 0);
    ui->lineEditBooksDir->setDisabled(idCurrentLib_ < 0);
    ui->listWidgetBooksDirs->setDisabled(idCurrentLib_ < 0);
    ui->btnExportLibrary->setDisabled(idCurrentLib_ < 0);
    ui->btnUpdateLibrary->setDisabled(idCurrentLib_ < 0);
    // установка доступности / недоступности контролов, в зависимости от числа итемов виджета списка папок
    SetEnabledOrDisabledControllsOfBooksDirs();
    QSettings* settings=GetSettings();
    ui->labelOPDS->setText(idCurrentLib_ < 0 ? "" : QString("<a href=\"http://localhost:%2/opds_%1\">http://localhost:%2/opds_%1</a>").arg(idCurrentLib_).arg(settings->value("OPDS_port",default_OPDS_port).toString()));
    ui->labelHTTP->setText(idCurrentLib_ < 0 ? "" : QString("<a href=\"http://localhost:%2/http_%1\">http://localhost:%2/http_%1</a>").arg(idCurrentLib_).arg(settings->value("OPDS_port",default_OPDS_port).toString()));

    settings->setValue("LibID", idCurrentLib_);
    //g_idCurrentLib = idCurrentLib_;
}

void AddLibrary::SaveLibrary(int idLib, SLib &Lib)
{
    QSqlQuery query(QSqlDatabase::database("libdb"));
    int idSaveLib = idLib;
    if(idLib<0)
    {
        LogMessage(tr("Add library"));
        bool result = query.exec(QString("INSERT INTO lib(name,path,inpx,firstAuthor,woDeleted) values('%1','%2','%3',%4,%5)").arg(Lib.name,Lib.path,Lib.sInpx,Lib.bFirstAuthor?"1":"0",Lib.bWoDeleted?"1":"0"));
        if(!result)
            qDebug()<<query.lastError().databaseText();
        idSaveLib = query.lastInsertId().toInt();
        QSettings* settings=GetSettings();
        settings->setValue("LibID",idLib);
        settings->sync();
    }
    else
    {
        LogMessage(tr("Update library"));
        bool result = query.exec(QString("UPDATE Lib SET name='%1',path='%2',inpx='%3' ,firstAuthor=%4, woDeleted=%5 WHERE ID=%6").arg(Lib.name,Lib.path,Lib.sInpx,Lib.bFirstAuthor?"1":"0",Lib.bWoDeleted?"1":"0").arg(idLib));
        if(!result)
            qDebug()<<query.lastError().databaseText();

    }
    mLibs[idSaveLib] = Lib;
    idCurrentLib_ = idSaveLib;
    UpdateLibList();
    SelectLibrary(idSaveLib);
    bLibChanged_ = true;
}

void AddLibrary::DeleteLibrary()
{
    if (idCurrentLib_ < 0)
        return;

    if (QMessageBox::question(
        this, tr("Delete library"),
        tr("Delete library") + " \"" + ui->comboBoxExistingLibs->currentText() + "\"",
        QMessageBox::Yes | QMessageBox::No, QMessageBox::No) == QMessageBox::No
        )
        return;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    ui->Log->clear();
    ClearLib(QSqlDatabase::database("libdb"),idCurrentLib_, false);
    QSqlQuery query(QSqlDatabase::database("libdb"));
    query.exec("DELETE FROM lib where ID=" + QString::number(idCurrentLib_));
    mLibs.remove(idCurrentLib_);
    UpdateLibList();
    if (ui->comboBoxExistingLibs->count() > 0) {
        ui->comboBoxExistingLibs->setCurrentIndex(0);
        SelectLibrary();
    }
    else {
        // установка контролов в состояние по-умолчанию, когда нет ни одной библиотеки
        SetControllsToDefaultState();
    }
    ui->btnSaveLog->setEnabled(ui->Log->count() > 1);
    bLibChanged_ = true;
    QApplication::restoreOverrideCursor();
}
void AddLibrary::EndUpdate()
{
    LogMessage(tr("Ending"));
    ui->btnExportLibrary->setDisabled(false);
    ui->btnUpdateLibrary->setDisabled(false);
    ui->btnCancel->setText(tr("Close"));
    ui->lineEditBooksDir->setDisabled(false);
    ui->lineEditInpxFile->setDisabled(false);
    ui->btnLibraryAdd->setDisabled(false);
    ui->btnLibraryEdit->setDisabled(false);
    ui->btnLibraryDelete->setDisabled(false);
    ui->comboBoxExistingLibs->setDisabled(false);
    ui->checkBoxFirstAuthorOnly->setDisabled(false);
    ui->checkBoxWoDeleted->setDisabled(false);
    ui->checkBoxShowLog->setChecked(false);
    ExpandLog();
    
    // загрузка полного лога в Log контрол
    ui->Log->clear();
    ui->Log->addItems(LogList_);
    ui->Log->setCurrentRow(ui->Log->count() - 1);
    LogList_.clear();

    ui->btnSaveLog->setEnabled(ui->Log->count() > 1);
    bLibChanged_ = true;
    QApplication::restoreOverrideCursor();

}
void AddLibrary::terminateImport()
{
    emit break_import();
}

void AddLibrary::reject()
{
    if (ui->btnCancel->text()==tr("Close"))
    {
        if(idCurrentLib_!=g_idCurrentLib){
            bLibChanged_ = true;
            g_idCurrentLib=idCurrentLib_;
        }
        QDialog::reject();
    }
    else
    {
        terminateImport();
    }
}

void AddLibrary::ExistingLibsChanged()
{
    ui->comboBoxExistingLibs->setItemText(ui->comboBoxExistingLibs->currentIndex(),ui->comboBoxExistingLibs->lineEdit()->text());
}

void AddLibrary::ExportLib()
{
    //accept();
    QString dirName = QFileDialog::getExistingDirectory(this, tr("Select destination directory"));
    if (dirName != "")
    {
        ExportDlg ed(this);
        ed.exec(idCurrentLib_, dirName);
    }
}

void AddLibrary::ButtonSaveLogClicked()
{
    if (ui->Log->count() > 0)
    {
        QString filePath = QFileDialog::getSaveFileName(this, tr("Save Log to file"), "", "*.txt");
        QStringList list;
        list << tr("Library:") + " " + ui->comboBoxExistingLibs->lineEdit()->text().trimmed();
        list << tr("Books dir:") + " " + ui->lineEditBooksDir->text().trimmed();
        for (int i = 0; i < ui->Log->count(); i++)
            list << ui->Log->item(i)->text();
        QFile file(filePath);
        if (file.open(QIODevice::WriteOnly | QIODevice::Text))
        {
            QTextStream out(&file);
            out.setCodec("UTF-8");
            for (const auto& row : list)
                out << row << endl;
            file.close();
            QMessageBox::information(this, tr("Save Log to File"), tr("Log saved to file."));
        }
    }
}

/*
    добавление нового каталога с книгами в список каталогов библиотеки
*/
void AddLibrary::AddBooksDirToList()
{
    QString BookDir = ui->lineEditBooksDir->text().trimmed();
    if (BookDir.isEmpty() || !QDir(BookDir).exists())
    {
        QMessageBox::critical(this, tr("Error"), tr("Specify the correct path to the books folder."));
        ui->lineEditBooksDir->setFocus();
        ui->lineEditBooksDir->selectAll();
        return;
    }
    else if (ui->listWidgetBooksDirs->findItems(BookDir, Qt::MatchFixedString).count() > 0)
    {
        QMessageBox::critical(this, tr("Error"), tr("This directory is already in the directory listing."));
        ui->lineEditBooksDir->setFocus();
        ui->lineEditBooksDir->selectAll();
        return;
    }

    // проверка, является ли добавляемый каталог одним из подкаталогов путей в списке
    for (int i = 0; i < ui->listWidgetBooksDirs->count(); ++i)
    {
        // QDir::separator() для случаев, когда в имени папки находится (.), которую contains() воспринимает, как разделитель,
        // что иногда приводит к неверным результатам сравнения в нашем случае: папка "_fb2.zip" ложно определяется, как вложенная в папку "_fb2"
        QString DirPath = ui->listWidgetBooksDirs->item(i)->text() + QDir::separator();
        QString tempBookDir = BookDir + QDir::separator();
        if (tempBookDir.contains(DirPath, Qt::CaseSensitive))
        {
            QMessageBox::critical(
                this, tr("Error"), tr("This directory is a sub-directory of one of the directories in the list.")
            );
            ui->lineEditBooksDir->setFocus();
            ui->lineEditBooksDir->selectAll();
            return;
        }
    }
    ui->listWidgetBooksDirs->addItem(BookDir);
    // установка доступности/недоступности контролов, в зависимости от числа итемов виджета списка папок
    SetEnabledOrDisabledControllsOfBooksDirs();
}

/*
    удаление выбранного каталога с книгами из списка каталогов библиотеки
*/
void AddLibrary::DeleteDirFromBookDirsList()
{
    QListWidgetItem* currentItem = ui->listWidgetBooksDirs->currentItem();
    if (currentItem != nullptr) {
        if (QMessageBox::question(
            this, tr("Delete dir from Dirs List"),
            tr("Books Dir:") + " \"" + currentItem->text() + "\"\n\n" +
            tr("Are you sure you want to remove the selected folder from the list of catalogs of books of the library?"),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No)
            == QMessageBox::Yes) {
            ui->listWidgetBooksDirs->takeItem(ui->listWidgetBooksDirs->currentRow());
            // установка доступности/недоступности контролов, в зависимости от числа итемов виджета списка папок
            SetEnabledOrDisabledControllsOfBooksDirs();
        }
    }
}

/*
    установка доступности/недоступности контролов, в зависимости от числа итемов виджета списка папок
*/
void AddLibrary::SetEnabledOrDisabledControllsOfBooksDirs()
{
    if (ui->listWidgetBooksDirs->count() > 0)
        ui->btnUpdateLibrary->setEnabled(true);
    else
        ui->btnUpdateLibrary->setDisabled(true);
}

/*
    обработчик сигнала выделения/снятия выделения итема списка книг библиотеки
*/
void AddLibrary::SelectionChangedBookDirsList(const QItemSelection& /*selected*/, const QItemSelection& /*deselected*/)
{
    // установка доступности/недоступности контролов, в зависимости от числа итемов виджета списка папок
    SetEnabledOrDisabledControllsOfSelectedStateItemBooksDirs();
}

/*
    установка доступности/недоступности контролов, в зависимости от наличия выделения итемов виджета списка папок
*/
void AddLibrary::SetEnabledOrDisabledControllsOfSelectedStateItemBooksDirs()
{
    if (ui->listWidgetBooksDirs->selectedItems().count() > 0)
        ui->btnBooksDirDelete->setEnabled(true);
    else
        ui->btnBooksDirDelete->setDisabled(true);
}

/*
    занесение в таблицу groups заблокированные от удаления/правки Группы
*/
void AddLibrary::AddGroupToSQLite(qlonglong libID)
{
    QSqlQuery query(QSqlDatabase::database("libdb"));

    // Избранное
    QPixmap favoritesPixmap(":/icons/img/icons/favorites.png");
    QByteArray favoritesByteArray;
    QBuffer favoritesBuffer(&favoritesByteArray);
    favoritesBuffer.open(QIODevice::WriteOnly);
    favoritesPixmap.save(&favoritesBuffer, "PNG");
    query.prepare("INSERT INTO groups(name, id_lib, blocked, blocked_name, icon) values(:name, :id_lib, :blocked, :blocked_name, :icon);");
    query.bindValue(":name", tr("Favorites"));
    query.bindValue(":id_lib", libID);
    query.bindValue(":blocked", true);
    query.bindValue(":blocked_name", "favorites");
    query.bindValue(":icon", favoritesByteArray);
    if (!query.exec())
        qDebug() << query.lastError().text();

    // К прочтению
    QPixmap toReadPixmap(":/icons/img/icons/toRead.png");
    QByteArray toReadByteArray;
    QBuffer toReadBuffer(&toReadByteArray);
    toReadBuffer.open(QIODevice::WriteOnly);
    toReadPixmap.save(&toReadBuffer, "PNG");
    query.prepare("INSERT INTO groups(name, id_lib, blocked, blocked_name, icon) values(:name, :id_lib, :blocked, :blocked_name, :icon);");
    query.bindValue(":name", tr("To read"));
    query.bindValue(":id_lib", libID);
    query.bindValue(":blocked", true);
    query.bindValue(":blocked_name", "toRead");
    query.bindValue(":icon", toReadByteArray);
    if (!query.exec())
        qDebug() << query.lastError().text();
   
    // Читаю
    QPixmap readPixmap(":/icons/img/icons/read.png");
    QByteArray readByteArray;
    QBuffer readBuffer(&readByteArray);
    readBuffer.open(QIODevice::WriteOnly);
    readPixmap.save(&readBuffer, "PNG");
    query.prepare("INSERT INTO groups(name, id_lib, blocked, blocked_name, icon) values(:name, :id_lib, :blocked, :blocked_name, :icon);");
    query.bindValue(":name", tr("I read"));
    query.bindValue(":id_lib", libID);
    query.bindValue(":blocked", true);
    query.bindValue(":blocked_name", "read");
    query.bindValue(":icon", readByteArray);
    if (!query.exec())
        qDebug() << query.lastError().text();
}

/*
    установка контролов в состояние по-умолчанию, когда нет ни одной библиотеки
*/
void AddLibrary::SetControllsToDefaultState()
{
    ui->lineEditInpxFile->clear();
    ui->lineEditBooksDir->clear();
    ui->listWidgetBooksDirs->clear();
    ui->rbtnAddNewBook->setChecked(true);
    ui->checkBoxFirstAuthorOnly->setChecked(false);
    ui->checkBoxWoDeleted->setChecked(false);

    ui->comboBoxExistingLibs->setDisabled(true);
    ui->lineEditInpxFile->setDisabled(true);
    ui->lineEditBooksDir->setDisabled(true);
    ui->listWidgetBooksDirs->setDisabled(true);
    ui->btnLibraryEdit->setDisabled(true);
    ui->btnLibraryDelete->setDisabled(true);
    ui->btnBooksDirAdd->setDisabled(true);
    ui->btnBooksDirDelete->setDisabled(true);
    ui->btnSaveLog->setDisabled(true);
    ui->btnUpdateLibrary->setDisabled(true);
    ui->btnExportLibrary->setDisabled(true);
}

/*
    обработчик сигнала изменения текста в контроле пути к каталогу с книгами
*/
void AddLibrary::LineEditBooksDirTextChanged(const QString& text)
{
    ui->btnBooksDirAdd->setEnabled(!text.trimmed().isEmpty());
}

/*
    расширить окно лога
*/
void AddLibrary::ExpandLog()
{
    ui->checkBoxShowLog->isChecked() ? ui->widgetBaseControlls->hide() : ui->widgetBaseControlls->show();
}

/*
    Есть ли проверяемая библиотека в списке библиотек?
*/
bool AddLibrary::IsLibraryWithNameExists(const QString & libraryName, const QString& messageTitle)
{
    QMap<int, SLib>::const_iterator iter = mLibs.constBegin();
    while (iter != mLibs.constEnd()) {
        if (iter.key() != -1)
            if (ui->comboBoxExistingLibs->findText(libraryName) != -1) {
                QMessageBox::critical(this, messageTitle,
                    tr("The entered name of the library:") + " '" + libraryName + "'.\n" +
                    tr("A library with this name already exists!") + "\n" +
                    tr("Enter another name for the library."));
                return true;
            }
        ++iter;
    }
    return false;
}
