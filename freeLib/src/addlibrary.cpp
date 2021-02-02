#include <QMainWindow>
#include <QToolButton>

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
    bLibChanged = false;
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

    idCurrentLib_ = idCurrentLib;
    UpdateLibList();

    connect(tbInpx,SIGNAL(clicked()),this,SLOT(InputINPX()));
    connect(tbBooksDir,SIGNAL(clicked()),this,SLOT(SelectBooksDir()));
    connect(ui->btnUpdateLibrary,SIGNAL(clicked()),this,SLOT(StartImport()));
    connect(ui->btnExportLibrary,SIGNAL(clicked()),this,SLOT(ExportLib()));
    connect(ui->comboBoxExistingLibs,SIGNAL(currentIndexChanged(int)),this,SLOT(SelectLibrary()));
    connect(ui->btnLibraryDelete,SIGNAL(clicked()),this,SLOT(DeleteLibrary()));
    connect(ui->btnLibraryAdd,SIGNAL(clicked()),this,SLOT(Add_Library()));
    connect(ui->comboBoxExistingLibs->lineEdit(),SIGNAL(editingFinished()),this,SLOT(ExistingLibsChanged()));
    connect(ui->lineEditBooksDir, &QLineEdit::textChanged, this, &AddLibrary::BookDirChanged);
    connect(ui->btnSaveLog, &QPushButton::clicked, this, &AddLibrary::ButtonSaveLogClicked);
    connect(ui->btnBooksDirAdd, &QToolButton::clicked, this, &AddLibrary::AddBooksDirToList);
    connect(ui->btnBooksDirDelete, &QToolButton::clicked, this, &AddLibrary::DeleteDirFromBookDirsList);
    ui->rbtnAddNewBook->setChecked(true);

    SelectLibrary(idCurrentLib_);
//    SelectLibrary();
    ui->btnUpdateLibrary->setDisabled(idCurrentLib_ < 0 || ui->lineEditBooksDir->text().trimmed().isEmpty());
}

AddLibrary::~AddLibrary()
{
    delete ui;
}

bool AddLibrary::IsLibraryChanged() const
{
    return bLibChanged;
}

void AddLibrary::Add_Library()
{
    ui->Log->clear();
    idCurrentLib_ =-1;
    QString sNewName = tr("new") + " ("+ QDateTime::currentDateTime().toString("dd.MM.yyyy HH:mm:ss") + ")";
    ui->comboBoxExistingLibs->blockSignals(true);
    ui->comboBoxExistingLibs->addItem(sNewName,-1);
    SLib lib;//{sNewName,"","",false,false};
    lib.name = sNewName;
    lib.bFirstAuthor = false;
    lib.bWoDeleted = false;
    SaveLibrary(idCurrentLib_,lib);
    ui->comboBoxExistingLibs->blockSignals(false);
    ui->comboBoxExistingLibs->setCurrentIndex(ui->comboBoxExistingLibs->count()-1);
    ui->btnUpdateLibrary->setDisabled(true);
    ui->btnSaveLog->setEnabled(ui->Log->count() > 1);
}

void AddLibrary::LogMessage(QString msg)
{
    while(ui->Log->count()>100)
        delete ui->Log->takeItem(0);
    ui->Log->addItem(msg);
    ui->Log->setCurrentRow(ui->Log->count()-1);
    m_LogList << msg;
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
    QString dir=QFileDialog::getExistingDirectory(this,tr("Select books directory"));
    if(!dir.isEmpty())
        ui->lineEditBooksDir->setText(dir);
}

void AddLibrary::UpdateLibList()

{
    if(!db_is_open)
        return;
    bool block = ui->comboBoxExistingLibs->blockSignals(true);
    ui->comboBoxExistingLibs->clear();
    auto i = mLibs.constBegin();
    while(i!=mLibs.constEnd()){
        ui->comboBoxExistingLibs->addItem(i->name,i.key());
        ++i;
    }
    ui->comboBoxExistingLibs->blockSignals(block);
}

void AddLibrary::StartImport()
{
    QString BookDir = ui->lineEditBooksDir->text().trimmed();
    if (BookDir == "" || !QDir(BookDir).exists())
    {
        QMessageBox::critical(this, tr("Error"), tr("Specify the correct path to the books folder."));
        ui->lineEditBooksDir->setFocus();
        ui->lineEditBooksDir->selectAll();
        return;
    }
    SLib lib;//{ui->comboBoxExistingLibs->currentText().trimmed(),ui->lineEditBooksDir->text().trimmed(),ui->lineEditInpxFile->text().trimmed(),
               // ui->checkBoxFirstAuthorOnly->isChecked(),ui->checkBoxWoDeleted->isChecked()};
    lib.name = ui->comboBoxExistingLibs->currentText().trimmed();
    lib.sInpx = ui->lineEditInpxFile->text().trimmed();
    lib.path = ui->lineEditBooksDir->text().trimmed();
    lib.bFirstAuthor = ui->checkBoxFirstAuthorOnly->isChecked();
    lib.bWoDeleted = ui->checkBoxWoDeleted->isChecked();
    StartImport(lib);
}

void AddLibrary::StartImport(SLib &Lib)
{
    QApplication::setOverrideCursor(QCursor(Qt::BusyCursor));
    m_LogList.clear();
    // UT_NEW: Добавить новые книги
    // UT_FULL: Пересоздать библиотеку
    // UT_DEL_AND_NEW : Удалить несуществующие и добавить новые книги
    int update_type=(ui->rbtnAddNewBook->isChecked()?UT_NEW:ui->rbtnDeleleOldBook->isChecked()?UT_DEL_AND_NEW:UT_FULL);
    SaveLibrary(idCurrentLib_,Lib);
    ui->btnExportLibrary->setDisabled(true);
    ui->btnUpdateLibrary->setDisabled(true);
    ui->lineEditBooksDir->setDisabled(true);
    ui->lineEditInpxFile->setDisabled(true);
    ui->comboBoxExistingLibs->setDisabled(true);
    ui->btnLibraryDelete->setDisabled(true);
    ui->btnLibraryAdd->setDisabled(true);
    ui->checkBoxFirstAuthorOnly->setDisabled(true);
    ui->checkBoxWoDeleted->setDisabled(true);
    ui->btnCancel->setText(tr("Break"));
    ui->groupBoxUpdateGroup->hide();
    ui->checkBoxFirstAuthorOnly->hide();
    ui->checkBoxWoDeleted->hide();

    thread = new QThread;
    imp_tr=new ImportThread();
    imp_tr->start(Lib.sInpx,Lib.name,Lib.path,idCurrentLib_,update_type,false,
                  Lib.bFirstAuthor&&Lib.sInpx.isEmpty(),Lib.bWoDeleted);
    imp_tr->moveToThread(thread);
    connect(imp_tr, SIGNAL(Message(QString)), this, SLOT(LogMessage(QString)));
    connect(thread, SIGNAL(started()), imp_tr, SLOT(process()));
    connect(imp_tr, SIGNAL(End()), thread, SLOT(quit()));
    connect(thread, SIGNAL(finished()), thread, SLOT(deleteLater()));
    connect(imp_tr, SIGNAL(End()), this, SLOT(EndUpdate()));
    connect(this, SIGNAL(break_import()), imp_tr, SLOT(break_import()));

    thread->start();
}

void AddLibrary::AddNewLibrary(SLib &lib)
{
    if(!db_is_open)
    {
        db_is_open=openDB(true,false);
    }
    idCurrentLib_ =-1;
    StartImport(lib);
    exec();
}


void AddLibrary::SelectLibrary(int idLib)
{
    bool block = ui->comboBoxExistingLibs->blockSignals(true);
    if(idLib>=0 && mLibs.count()>0){
        for(int i=0;i<ui->comboBoxExistingLibs->count();i++){
            if(ui->comboBoxExistingLibs->itemData(i).toInt()==idCurrentLib_){
                ui->comboBoxExistingLibs->setCurrentIndex(i);
                ui->lineEditBooksDir->setText(mLibs[idCurrentLib_].path);
                ui->lineEditInpxFile->setText(mLibs[idCurrentLib_].sInpx);
                ui->checkBoxFirstAuthorOnly->setChecked(mLibs[idCurrentLib_].bFirstAuthor);
                ui->checkBoxWoDeleted->setChecked(mLibs[idCurrentLib_].bWoDeleted);
                QSettings* settings=GetSettings();
                ui->labelOPDS->setText(idCurrentLib_<0?"":QString("<a href=\"http://localhost:%2/opds_%1\">http://localhost:%2/opds_%1</a>").arg(idCurrentLib_).arg(settings->value("OPDS_port",default_OPDS_port).toString()));
                ui->labelHTTP->setText(idCurrentLib_<0?"":QString("<a href=\"http://localhost:%2/http_%1\">http://localhost:%2/http_%1</a>").arg(idCurrentLib_).arg(settings->value("OPDS_port",default_OPDS_port).toString()));
                break;
            }
        }
    }
    ui->btnLibraryDelete->setDisabled(idLib<0);
    ui->comboBoxExistingLibs->setDisabled(idLib<0);
    ui->lineEditInpxFile->setDisabled(idLib<0);
    ui->lineEditBooksDir->setDisabled(idLib<0);
    ui->btnExportLibrary->setDisabled(idLib<0);
    ui->btnUpdateLibrary->setDisabled(idLib<0);
    ui->comboBoxExistingLibs->blockSignals(block);
}

void AddLibrary::SelectLibrary()
{
    int nIndex = ui->comboBoxExistingLibs->currentIndex();
    QString dir,inpx;
    bool firstAuthor=false;
    bool bWoDeleted = false;
    if(nIndex>=0)
        idCurrentLib_ = ui->comboBoxExistingLibs->itemData(nIndex).toInt();
    if(idCurrentLib_>=0){
        dir = mLibs[idCurrentLib_].path;
        inpx = mLibs[idCurrentLib_].sInpx;
        firstAuthor = mLibs[idCurrentLib_].bFirstAuthor;
        bWoDeleted = mLibs[idCurrentLib_].bWoDeleted;
    }

    ui->lineEditBooksDir->setText(dir);
    ui->lineEditInpxFile->setText(inpx);
    ui->checkBoxFirstAuthorOnly->setChecked(firstAuthor);
    ui->checkBoxWoDeleted->setChecked(bWoDeleted);
    ui->btnLibraryDelete->setDisabled(idCurrentLib_<0);
    ui->comboBoxExistingLibs->setDisabled(idCurrentLib_<0);
    ui->lineEditInpxFile->setDisabled(idCurrentLib_<0);
    ui->lineEditBooksDir->setDisabled(idCurrentLib_<0);
    ui->btnExportLibrary->setDisabled(idCurrentLib_ < 0);
    ui->btnUpdateLibrary->setDisabled(idCurrentLib_ < 0 || ui->lineEditBooksDir->text().trimmed().isEmpty());
    QSettings* settings=GetSettings();
    ui->labelOPDS->setText(idCurrentLib_<0?"":QString("<a href=\"http://localhost:%2/opds_%1\">http://localhost:%2/opds_%1</a>").arg(idCurrentLib_).arg(settings->value("OPDS_port",default_OPDS_port).toString()));
    ui->labelHTTP->setText(idCurrentLib_<0?"":QString("<a href=\"http://localhost:%2/http_%1\">http://localhost:%2/http_%1</a>").arg(idCurrentLib_).arg(settings->value("OPDS_port",default_OPDS_port).toString()));

    settings->setValue("LibID",idCurrentLib_);
    //idCurrentLib = idCurrentLib_;
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
    bLibChanged = true;
 }
void AddLibrary::DeleteLibrary()
{
    if(idCurrentLib_<0)
        return;

    if(QMessageBox::question(this,tr("Delete library"),tr("Delete library")+" \""+ui->comboBoxExistingLibs->currentText()+"\"",QMessageBox::Yes|QMessageBox::No,QMessageBox::No)==QMessageBox::No)
        return;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    ui->Log->clear();
    ClearLib(QSqlDatabase::database("libdb"),idCurrentLib_,false);
    QSqlQuery query(QSqlDatabase::database("libdb"));
    query.exec("DELETE FROM lib where ID="+QString::number(idCurrentLib_));
    mLibs.remove(idCurrentLib_);
    UpdateLibList();
    if(ui->comboBoxExistingLibs->count()>0){
        ui->comboBoxExistingLibs->setCurrentIndex(0);
        SelectLibrary();
    }
    ui->btnSaveLog->setEnabled(ui->Log->count() > 1);
    bLibChanged = true;
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
    ui->btnLibraryDelete->setDisabled(false);
    ui->btnLibraryAdd->setDisabled(false);
    ui->comboBoxExistingLibs->setDisabled(false);
    ui->checkBoxFirstAuthorOnly->setDisabled(false);
    ui->checkBoxWoDeleted->setDisabled(false);
    ui->groupBoxUpdateGroup->show();
    ui->checkBoxFirstAuthorOnly->show();
    ui->checkBoxWoDeleted->show();
    
    // загрузка полного лога в Log контрол
    ui->Log->clear();
    ui->Log->addItems(m_LogList);
    ui->Log->setCurrentRow(ui->Log->count() - 1);
    m_LogList.clear();

    ui->btnSaveLog->setEnabled(ui->Log->count() > 1);
    bLibChanged = true;
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
        if(idCurrentLib_!=idCurrentLib){
            bLibChanged = true;
            idCurrentLib=idCurrentLib_;
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

void AddLibrary::BookDirChanged(const QString& text)
{
    if (ui->lineEditBooksDir->text().trimmed().isEmpty())
        ui->btnUpdateLibrary->setDisabled(true);
    else
        ui->btnUpdateLibrary->setDisabled(false);
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
    добавление нового каталога с книгами в список каталогов
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
    ui->listWidgetBooksDirs->addItem(BookDir);
    ui->btnBooksDirDelete->setEnabled(true);
}

/*
    удаление выбранного каталога с книгами из списка каталогов библиотеки
*/
void AddLibrary::DeleteDirFromBookDirsList()
{
    QListWidgetItem *currentItem = ui->listWidgetBooksDirs->currentItem();
    if (currentItem != nullptr)
    {
        if (QMessageBox::question(
            this, tr("Delete dir from Dirs List"),
            tr("Books Dir:") + " \"" + currentItem->text() + "\"\n\n" +
            tr("Are you sure you want to remove the selected folder from the list of catalogs of books of the library?"),
            QMessageBox::Yes | QMessageBox::No, QMessageBox::No)
            == QMessageBox::No)
            return;
        ui->listWidgetBooksDirs->takeItem(ui->listWidgetBooksDirs->currentRow());
        if (ui->listWidgetBooksDirs->count() == 0)
            ui->btnBooksDirDelete->setDisabled(true);
    }
}
