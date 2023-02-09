#ifndef STATISTICSDIALOG_H
#define STATISTICSDIALOG_H

#include <QtWidgets/QDialog>

namespace Ui {
class StatisticsDialog;
}

class StatisticsDialog : public QDialog
{
    Q_OBJECT
    
public:
    explicit StatisticsDialog(QWidget *parent = 0);
    ~StatisticsDialog();
    
private:
    Ui::StatisticsDialog *ui;

    QString GetLibraryInfo(uint idLibrary);
    QString GetAllDataInfo(uint idLibrary);
    QString GetAuthorsInfo(uint idLibrary);
    QString GetGenresInfo(uint idLibrary);
    QString GetSeriasInfo(uint idLibrary);
    QString GetBooksInfo(uint idLibrary);
    QString GetGroupsInfo(uint idLibrary);
};

#endif // STATISTICSDIALOG_H
