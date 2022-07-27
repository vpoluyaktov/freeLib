#ifndef EXPORTTHREAD_H
#define EXPORTTHREAD_H

#include <QThread>

#include "common.h"
#include "SmtpClient/smtpclient.h"

class ExportThread : public QObject
{
    Q_OBJECT
public:
    explicit ExportThread(QObject* parent = 0);
public:
    bool loop_enable;
    QList<qlonglong> successful_export_books;
    void start(QString _export_dir, const QList<book_info>& list_books, SendType send, qlonglong id_author);
    void start(QString _export_dir, const QStringList& list_books, SendType send);
    void start(qlonglong id_lib, QString path);
private:
    QList<book_info> book_list;
    QStringList book_list_str;
    SendType send_type;
    qlonglong IDauthor;
    QString export_dir;
    qlonglong ID_lib;
    void FB2export();
    void export_books();
    void export_lib();
    bool convert(QList<QBuffer*> outbuff, QString file_name, int count, bool remove_old, book_info& bi);
public slots:
    void break_exp();
    void process();
    //void smtpError(SmtpError e);
signals:
    void End();
    void Progress(int procent, int count);
};

#endif // EXPORTTHREAD_H
