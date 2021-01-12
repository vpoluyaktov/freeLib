#include <QDebug>
#include <QTextBrowser>
#include <QFile>

#include "helpdialog.h"
#include "ui_helpdialog.h"

HelpDialog::HelpDialog(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::HelpDialog)
{
    ui->setupUi(this);

    QFile fileAbout(QApplication::applicationDirPath() + "/Help/about.html");
    if (fileAbout.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        ui->webViewAbout->setHtml(fileAbout.readAll());
    }
    QFile fileChangeLog(QApplication::applicationDirPath() + "/Help/changelog.html");
    if (fileChangeLog.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        ui->webViewChangeLog->setHtml(fileChangeLog.readAll());
    }
    QFile fileParams(QApplication::applicationDirPath() + "/Help/params.html");
    if (fileParams.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        ui->webViewParametrs->setHtml(fileParams.readAll());
    }
    QFile fileCommandLineParametrs(QApplication::applicationDirPath() + "/Help/cmd_params.html");
    if (fileCommandLineParametrs.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        ui->webViewCommandLineParametrs->setHtml(fileCommandLineParametrs.readAll());
    }
}

HelpDialog::~HelpDialog()
{
    delete ui;
}
