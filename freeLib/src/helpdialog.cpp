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

    QFile file(QApplication::applicationDirPath() + "/Help/index.html");
    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        ui->webView->setHtml(file.readAll());
    }
}

HelpDialog::~HelpDialog()
{
    delete ui;
}
