#include "fontframe.h"
#include "ui_fontframe.h"

#include <QDir>
#include <QFontDatabase>
#include <QDebug>
#include <QFileDialog>

#include "common.h"
#include "utilities.h"

extern QList<tag> tag_list;

typedef struct _tagTT_OFFSET_TABLE{
    quint16	uMajorVersion;
    quint16	uMinorVersion;
    quint16	uNumOfTables;
    quint16	uSearchRange;
    quint16	uEntrySelector;
    quint16	uRangeShift;
}TT_OFFSET_TABLE;

typedef struct _tagTT_TABLE_DIRECTORY{
    char	szTag[4];			//table name
    quint32	uCheckSum;			//Check sum
    quint32	uOffset;			//Offset from beginning of file
    quint32	uLength;			//length of the table in bytes
}TT_TABLE_DIRECTORY;

typedef struct _tagTT_NAME_TABLE_HEADER{
    quint16	uFSelector;			//format selector. Always 0
    quint16	uNRCount;			//Name Records count
    quint16	uStorageOffset;		//Offset for strings storage, from start of the table
}TT_NAME_TABLE_HEADER;

typedef struct _tagTT_NAME_RECORD{
    quint16	uPlatformID;
    quint16	uEncodingID;
    quint16	uLanguageID;
    quint16	uNameID;
    quint16	uStringLength;
    quint16	uStringOffset;	//from start of storage area
}TT_NAME_RECORD;

typedef struct _tagTT_HEAD_RECORD{
    quint32	uVersion;
    quint32	uRevision;
    quint32	ucheckSum;
    quint32	umagicNumber;
    quint16	uflags;
    quint16	uunitsPerEm;
    quint64	ucreated;
    quint64	umodified;
    qint16	xMin;
    qint16	yMin;
    qint16	xMax;
    qint16	yMax;
    quint16	umacStyle;
    quint16	ulowestRecPPEM;
    qint16	fontDirectionHint;
    qint16	indexToLocFormat;
    qint16	glyphDataFormat;
}TT_HEAD_RECORD;

#define BYTE    quint8
#define WORD    quint16
#define DWORD   quint32
#define LOWORD(l) ((WORD)(l))
#define HIWORD(l) ((WORD)(((DWORD)(l) >> 16) & 0xFFFF))
#define LOBYTE(w) ((BYTE)(w))
#define HIBYTE(w) ((BYTE)(((WORD)(w) >> 8) & 0xFF))

#define MAKEWORD(low,high) ((WORD)((BYTE)(low)) | (((WORD)(BYTE)(high))<<8))
#define MAKELONG(a, b) ((DWORD) (((WORD) (a)) | ((DWORD) ((WORD) (b))) << 16))

#define SWAPWORD(x)		MAKEWORD(HIBYTE(x), LOBYTE(x))
#define SWAPLONG(x)		MAKELONG(SWAPWORD(HIWORD(x)), SWAPWORD(LOWORD(x)))

QString GetFontNameFromFile(QString lpszFilePath)
{
    QFile f(lpszFilePath);
    QString csRetVal=QApplication::translate("FontFrame","not found");

    if(f.open(QFile::ReadOnly))
    {
        TT_OFFSET_TABLE ttOffsetTable;
        f.read((char*)&ttOffsetTable, sizeof(TT_OFFSET_TABLE));
        ttOffsetTable.uNumOfTables = SWAPWORD(ttOffsetTable.uNumOfTables);
        ttOffsetTable.uMajorVersion = SWAPWORD(ttOffsetTable.uMajorVersion);
        ttOffsetTable.uMinorVersion = SWAPWORD(ttOffsetTable.uMinorVersion);

        //check is this is a true type font and the version is 1.0
        if(ttOffsetTable.uMajorVersion != 1 || ttOffsetTable.uMinorVersion != 0)
            return csRetVal;

        TT_TABLE_DIRECTORY tblDir;
        //quint32 HeadOffset=0;
        quint32 NameOffset=0;
       // bool bFound = false;
        QString csTemp;

        for(int i=0; i< ttOffsetTable.uNumOfTables; i++)
        {
            f.read((char*)&tblDir, sizeof(TT_TABLE_DIRECTORY));
            csTemp=QString::fromLatin1(tblDir.szTag,4);
            if(csTemp.toLower()=="name")
            {
                tblDir.uLength = SWAPLONG(tblDir.uLength);
                tblDir.uOffset = SWAPLONG(tblDir.uOffset);
                NameOffset=tblDir.uOffset;
            }
            if(NameOffset)
                break;
        }

        if(NameOffset)
        {
            //qDebug()<<"name "<<NameOffset;
            f.seek(NameOffset);
            TT_NAME_TABLE_HEADER ttNTHeader;
            f.read((char*)&ttNTHeader, sizeof(TT_NAME_TABLE_HEADER));
            ttNTHeader.uNRCount = SWAPWORD(ttNTHeader.uNRCount);
            ttNTHeader.uStorageOffset = SWAPWORD(ttNTHeader.uStorageOffset);
            TT_NAME_RECORD ttRecord;

            for(int i=0; i<ttNTHeader.uNRCount; i++)
            {
                f.read((char*)&ttRecord, sizeof(TT_NAME_RECORD));
                ttRecord.uNameID = SWAPWORD(ttRecord.uNameID);
                if(ttRecord.uNameID == 4)
                {
                    ttRecord.uStringLength = SWAPWORD(ttRecord.uStringLength);
                    ttRecord.uStringOffset = SWAPWORD(ttRecord.uStringOffset);
                    int nPos = f.pos();
                    f.seek(NameOffset + ttRecord.uStringOffset + ttNTHeader.uStorageOffset);
                    char buf[1024];
                    f.read(buf, ttRecord.uStringLength);
                    csTemp=QString::fromLatin1(buf,ttRecord.uStringLength);
                    //csTemp.ReleaseBuffer();
                    if(!csTemp.isEmpty())
                    {
                        csRetVal = csTemp;
                        break;
                    }
                    f.seek(nPos);
                }
            }
        }
        f.close();

    }
    return csRetVal;
}

FontFrame::FontFrame(bool use, int tag, QString font, QString font_b, QString font_i, QString font_bi, int fontSize, QWidget *parent) :
    QFrame(parent),
    ui(new Ui::FontFrame)
{
    ui->setupUi(this);

    for(int i=0;i<tag_list.count();i++)
    {
        ui->tag->addItem(tag_list[i].name);
    }

    connect(ui->Use,SIGNAL(toggled(bool)),this,SLOT(UseChange(bool)));
    connect(ui->del,SIGNAL(clicked()),this,SLOT(DelPress()));
    connect(ui->Up,SIGNAL(clicked()),this,SLOT(UpPress()));
    connect(ui->Down,SIGNAL(clicked()),this,SLOT(DownPress()));
    connect(ui->font,SIGNAL(currentTextChanged(QString)),this,SLOT(FontSelected(QString)));
    connect(ui->font_b,SIGNAL(currentTextChanged(QString)),this,SLOT(FontSelected(QString)));
    connect(ui->font_i,SIGNAL(currentTextChanged(QString)),this,SLOT(FontSelected(QString)));
    connect(ui->font_bi,SIGNAL(currentTextChanged(QString)),this,SLOT(FontSelected(QString)));


    ui->font->addItem("");
    ui->font_b->addItem("");
    ui->font_i->addItem("");
    ui->font_bi->addItem("");

    ui->Use->setChecked(use);
    ui->tag->setCurrentIndex(tag);
    ui->fontSize->setValue(fontSize);
    if(font.isEmpty())
        on_tag_currentIndexChanged(tag);


    QDir dir(QApplication::applicationDirPath()+"/xsl/fonts");
    foreach(QString str,dir.entryList(QStringList()<<"*.ttf",QDir::Files|QDir::NoSymLinks|QDir::NoDotAndDotDot|QDir::Readable,QDir::Name))
    {
        ui->font->addItem(GetFontNameFromFile(dir.absoluteFilePath(str)),str);
        ui->font_b->addItem(GetFontNameFromFile(dir.absoluteFilePath(str)),str);
        ui->font_i->addItem(GetFontNameFromFile(dir.absoluteFilePath(str)),str);
        ui->font_bi->addItem(GetFontNameFromFile(dir.absoluteFilePath(str)),str);
    }
    QString HomeDir="";
    if(QStandardPaths::standardLocations(QStandardPaths::HomeLocation).count()>0)
        HomeDir=QStandardPaths::standardLocations(QStandardPaths::HomeLocation).at(0);

    QSettings  *settings=GetSettings();
    QString db_path=QFileInfo(settings->value("database_path",HomeDir+"/freeLib/freeLib.sqlite").toString()).absolutePath()+"/fonts";
    dir.setPath(db_path);
    foreach(QString str,dir.entryList(QStringList()<<"*.ttf",QDir::Files|QDir::NoSymLinks|QDir::NoDotAndDotDot|QDir::Readable,QDir::Name))
    {
        ui->font->addItem(GetFontNameFromFile(dir.absoluteFilePath(str)),str);
        ui->font_b->addItem(GetFontNameFromFile(dir.absoluteFilePath(str)),str);
        ui->font_i->addItem(GetFontNameFromFile(dir.absoluteFilePath(str)),str);
        ui->font_bi->addItem(GetFontNameFromFile(dir.absoluteFilePath(str)),str);
    }

    if(font!="..." && !font.isEmpty())
    {
        for(int i=0;i<4;i++)
        {
            bool find=false;
            QComboBox *font_box=0;
            QString cur_font;
            switch(i)
            {
            case 0:
                font_box=ui->font;
                cur_font=font;
                break;
            case 1:
                font_box=ui->font_b;
                cur_font=font_b;
                break;
            case 2:
                font_box=ui->font_i;
                cur_font=font_i;
                break;
            case 3:
                font_box=ui->font_bi;
                cur_font=font_bi;
                break;
            }

            for(int i=0;i<font_box->count();i++)
            {

                if(font_box->itemData(i).toString()==cur_font)
                {
                    find=true;
                    font_box->setCurrentIndex(i);
                    break;
                }
            }
            if(!find)
            {
                QString name=GetFontNameFromFile(cur_font);
                font_box->addItem(name,cur_font);
                font_box->setCurrentIndex(font_box->count()-1);
            }
        }
    }
    ui->font->addItem("...");
    ui->font_b->addItem("...");
    ui->font_i->addItem("...");
    ui->font_bi->addItem("...");
    current_font=ui->font->currentIndex();
    current_font_b=ui->font_b->currentIndex();
    current_font_i=ui->font_i->currentIndex();
    current_font_bi=ui->font_bi->currentIndex();
    UseChange(use);
}

void FontFrame::FontSelected(QString str)
{
    QComboBox *font_box=(QComboBox*)sender();
    disconnect(font_box,SIGNAL(currentTextChanged(QString)),this,SLOT(FontSelected(QString)));
    if(str=="...")
    {
        QString fileName = QFileDialog::getOpenFileName(this, tr("Select font"),"",tr("Fonts")+" (*.ttf)");
        if(!fileName.isEmpty())
        {
            bool find=false;
            for(int i=0;i<font_box->count();i++)
            {

                if(font_box->itemData(i).toString()==fileName)
                {
                    find=true;
                    font_box->setCurrentIndex(i);
                    break;
                }
            }
            if(!find)
            {
                QString name=GetFontNameFromFile(fileName);
                font_box->insertItem(0,name,fileName);
                font_box->setCurrentIndex(0);
            }
        }
        else
            font_box->setCurrentIndex(current_font);
    }
    current_font=font_box->currentIndex();
    connect(font_box,SIGNAL(currentTextChanged(QString)),this,SLOT(FontSelected(QString)));
}

FontFrame::~FontFrame()
{
    delete ui;
}

void FontFrame::UseChange(bool state)
{
    for(int i=0;i<ui->horizontalLayout->count();i++)
    {
        if(ui->horizontalLayout->itemAt(i)->widget())
        {
            if(ui->horizontalLayout->itemAt(i)->widget()!=ui->Use)
                ui->horizontalLayout->itemAt(i)->widget()->setEnabled(state);
        }
    }
}

bool FontFrame::use()
{
    return ui->Use->isChecked();
}
int FontFrame::tag()
{
    return ui->tag->currentIndex();
}
QString FontFrame::font()
{
    return ui->font->currentData().toString();
}
QString FontFrame::font_b()
{
    return ui->font_b->currentData().toString();
}
QString FontFrame::font_i()
{
    return ui->font_i->currentData().toString();
}
QString FontFrame::font_bi()
{
    return ui->font_bi->currentData().toString();
}

void FontFrame::DelPress()
{
    emit remove_font(this);
}

void FontFrame::UpPress()
{
    emit move_font(this,1);
}

void FontFrame::DownPress()
{
    emit move_font(this,-1);
}

int FontFrame::fontSize()
{
    return ui->fontSize->value();
}

void FontFrame::on_tag_currentIndexChanged(int index)
{
    ui->fontSize->setValue(tag_list[index].font_size);
}
