#include <QApplication>
#include <QDesktopWidget>
#include <QNetworkProxy>
#include <QStyleFactory>
#include <QLocale>
#include <QTextCodec>
#include <QTranslator>
#include <QLibraryInfo>
#include <QSplashScreen>
#include <QPainter>
#include <QMap>
#include <locale>

#include "fb2mobi/hyphenations.h"
#include <quazip/quazip/quazip.h>

#include "mainwindow.h"
#include "aboutdialog.h"
#include "common.h"
#include "library.h"
#include "build_number.h"

QSettings* global_settings = nullptr;
int g_idCurrentLib;
bool g_db_is_open;
QTranslator* translator;
QTranslator* translator_qt;
QCommandLineParser CMDparser;
QSplashScreen *splash;
QApplication *app;

int main(int argc, char *argv[])
{
    Q_INIT_RESOURCE(resource);

#ifdef Q_OS_MACX
    if ( QSysInfo::MacintoshVersion > QSysInfo::MV_10_8 ) {
        // fix Mac OS X 10.9 (mavericks) font issue
        // https://bugreports.qt-project.org/browse/QTBUG-32789
        //QFont::insertSubstitution(".Lucida Grande UI", "Lucida Grande");
    }
#endif


    QApplication a(argc, argv);
    a.setStyleSheet("QComboBox { combobox-popup: 0; }");

    a.setOrganizationName("freeLib");
    a.setApplicationName("freeLib");

    QCommandLineOption trayOption("tray", "minimize to tray on start");
    CMDparser.addOption(trayOption);
    CMDparser.process(a);

    QPalette darkPalette;
    darkPalette.setColor(QPalette::Window, QColor("#E0E0E0")); //основной цвет интер
    darkPalette.setColor(QPalette::WindowText, Qt::black);
    darkPalette.setColor(QPalette::Base, QColor("#FfFfFf"));
    darkPalette.setColor(QPalette::AlternateBase, QColor("#F0F0F0"));
    darkPalette.setColor(QPalette::ToolTipBase, Qt::black);
    darkPalette.setColor(QPalette::ToolTipText, Qt::black);
    darkPalette.setColor(QPalette::Text, Qt::black);
    darkPalette.setColor(QPalette::Button, QColor("#e4e4e4"));
    darkPalette.setColor(QPalette::ButtonText, Qt::black);
    //darkPalette.setColor(QPalette::BrightText, Qt::red);

    darkPalette.setColor(QPalette::Light, QColor("#c0c0c0"));
    darkPalette.setColor(QPalette::Midlight, QColor("#b0b0b0"));
    darkPalette.setColor(QPalette::Dark, QColor("#a0a0a0a"));
    darkPalette.setColor(QPalette::Mid, QColor("#909090"));
    darkPalette.setColor(QPalette::Shadow, QColor("#707070"));

    darkPalette.setColor(QPalette::Highlight, QColor("#0B61A4"));
    darkPalette.setColor(QPalette::HighlightedText, Qt::white);

    //darkPalette.setColor(QPalette::Link, QColor(42, 130, 218));
    //darkPalette.setColor(QPalette::LinkVisited, QColor(42, 130, 218));

    //a.setPalette(darkPalette);

    translator = nullptr;
    translator_qt = nullptr;
    app = &a;
    app->setAttribute(Qt::AA_UseHighDpiPixmaps);
    QSqlDatabase::addDatabase("QSQLITE", "libdb");

    SetLocale();

    QString HomeDir = "";
    if (QStandardPaths::standardLocations(QStandardPaths::HomeLocation).count() > 0)
        HomeDir = QStandardPaths::standardLocations(QStandardPaths::HomeLocation).at(0);
    QDir::setCurrent(HomeDir);
    QString sDirTmp = QString("%1/freeLib").arg(QStandardPaths::standardLocations(QStandardPaths::TempLocation).constFirst());
    QDir dirTmp(sDirTmp);
    if (!dirTmp.exists())
        dirTmp.mkpath(sDirTmp);


    QPixmap pixmap(QString(":/splash%1.png").arg(app->devicePixelRatio() >= 2 ? "@2x" : ""));
    pixmap.setDevicePixelRatio(app->devicePixelRatio());
    QPainter painter(&pixmap);
    painter.setFont(QFont(painter.font().family(), VERSION_FONT, QFont::Bold));
    painter.setPen(Qt::white);
    painter.drawText(QRect(30, 140, 360, 111), Qt::AlignLeft | Qt::AlignVCenter, PROG_VERSION);
    splash = new QSplashScreen(pixmap);
    splash->resize(640, 400);
//    QRect screenRect=QDesktopWidget().screenGeometry();
//    splash->move(
//                (screenRect.width()-splash->width())/2,
//                (screenRect.height()-splash->height())/2);
#ifdef Q_OS_LINUX
    splash->setWindowIcon(QIcon(":/library_128x128.png"));
#endif
    QSettings settings;
    if(!settings.contains("ApplicationMode")) {
        ResetToDefaultSettings();
    }

    if(!settings.value("no_splash",false).toBool())
        splash->show();
    a.processEvents();
    setProxy();
    //g_idCurrentLib=settings->value("LibID",-1).toInt();
    LibrarySQLiteWorker::UpdateLibs();
    MainWindow w;
#ifdef Q_OS_OSX
  //  w.setWindowFlags(w.windowFlags() & ~Qt::WindowFullscreenButtonHint);
#endif

    if (!w.IsErrorQuit()) {
        if (!CMDparser.isSet("tray") && settings.value("tray_icon", 0).toInt() != 2)
            w.show();
    } else {
        return 1;
    }
    splash->finish(&w);
    //current_lib.UpdateLib();
    if (g_idCurrentLib < 0 && settings.value("ApplicationMode", 0).toInt() == 0)
        w.ManageLibrary();
    int result = a.exec();
    if (global_settings) {
        global_settings->sync();
        delete global_settings;
    }

    return result;
}
