#include "utilities.h"

#include <QNetworkProxy>
#include <QDomDocument>

#include <quazip/quazip/quazipfile.h>

QNetworkProxy proxy;
QList<tag> tag_list;
quint8 key[] = { 1,65,245,245,235,2,34,61,0,32,54,12,66 };

void setProxy()
{
    QSettings* settings = GetSettings();
    proxy.setPort(static_cast<ushort>(settings->value("proxy_port", default_proxy_port).toInt()));
    proxy.setHostName(settings->value("proxy_host").toString());
    proxy.setPassword(settings->value("proxy_password").toString());
    proxy.setUser(settings->value("proxy_user").toString());
    switch (settings->value("proxy_type", 0).toInt()) {
    case 0:
        proxy.setType(QNetworkProxy::NoProxy);
        break;
    case 1:
        proxy.setType(QNetworkProxy::Socks5Proxy);
        break;
    case 2:
        proxy.setType(QNetworkProxy::HttpProxy);
        break;
    }
    QNetworkProxy::setApplicationProxy(proxy);
}

bool openDB(bool create, bool replace)
{
    QString sAppDir, db_file;
    //QSettings *settings=GetSettings();
    QSettings settings;

    QFileInfo fi(RelativeToAbsolutePath(settings.value("database_path").toString()));
    if (fi.exists() && fi.isFile()) {
        db_file = fi.canonicalFilePath();
    }
    else {
        sAppDir = QStandardPaths::standardLocations(QStandardPaths::AppDataLocation).constFirst();
        db_file = sAppDir + "/freeLib.sqlite";
        settings.setValue("database_path", db_file);
    }
    QFile file(db_file);
    if (!file.exists() || replace) {
        if (replace) {
            QSqlDatabase dbase = QSqlDatabase::database("libdb", true);
            dbase.close();
            if (!file.remove()) {
                qDebug() << ("Can't remove old database");
                g_db_is_open = false;
                return false;
            }
        }
        if (!create && !replace) {
            g_db_is_open = false;
            return true;
        }
        QDir dir;
        dir.mkpath(QFileInfo(db_file).absolutePath());
        file.setFileName(":/freeLib.sqlite");
        file.open(QFile::ReadOnly);
        QByteArray data = file.readAll();
        file.close();
        file.setFileName(db_file);
        file.open(QFile::WriteOnly);
        file.write(data);
        file.close();
    }
    QSqlDatabase dbase = QSqlDatabase::database("libdb", false);
    dbase.setDatabaseName(db_file);
    if (!dbase.open()) {
        qDebug() << ("Error connect! ") << db_file;
        g_db_is_open = false;
        return false;
    }
    g_db_is_open = true;
    qDebug() << "Open DB OK. " << db_file;
    return true;
}

QSettings* GetSettings(bool need_copy, bool reopen)
{
    if (reopen && global_settings) {
        global_settings->sync();
        delete global_settings;
        global_settings = nullptr;
    }
    if (global_settings && !need_copy) {
        global_settings->sync();
        return global_settings;
    }
    QSettings* current_settings;
    current_settings = new QSettings();
    if (need_copy)
        return current_settings;
    global_settings = current_settings;
    return global_settings;
}

/*
    связывание кнопки с быстрыми клавишами
*/
void BindAnyButtonShortcut(QAbstractButton* button, const QKeySequence& shortcut)
{
    QObject::connect(new QShortcut(shortcut, button), &QShortcut::activated, [button]() { button->animateClick(); });
}

/*
    удаление 'пустого' элемента структуры жанров, который появаляется в результате глюка в Qt по прямому доступу к элементу QMap по его индексу
*/
bool EraseEmptyGenreElement()
{
    if (mGenre[0].sCode.isEmpty()) {
        for (auto it = mGenre.begin(); it != mGenre.end();)
            if (it.value().sCode.isEmpty()) {
                it = mGenre.erase(it);
                return true;
            }
            else
                ++it;
    }
    return false;
}

/*
    перемещение QListWidgetItem вверх в QListWidget
*/
void MoveQListWidgetItemToUp(QListWidget* lw, const QStringList& list)
{
    for (int i = 0; i <= 2; i++) {
        QListWidgetItem* item = lw->findItems(list[i], Qt::MatchCaseSensitive)[0];
        int row = lw->row(item);
        QListWidgetItem* itemForInsert = lw->takeItem(row);
        lw->insertItem(i, itemForInsert);
    }
}

/*
    получение данных о файле
*/
QFileInfo GetBookFile(QBuffer& buffer_book, QBuffer& buffer_info, uint id_book, bool caption, QDateTime* file_data)
{
    QFileInfo fi;
    SBook& book = mLibs[g_idCurrentLib].mBooks[id_book];
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
            *file_data = fi.birthTime();
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
        foreach(QuaZipFileInfo64 str, list) {
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
    QPixmap pixmap(size, size - 4);
    pixmap.fill(Qt::transparent);
    QPainter paint(&pixmap);
    paint.setBrush(QBrush(color));
    QPen pen = QPen(QColor(
        static_cast<int>(color.red() * 0.5),
        static_cast<int>(color.green() * 0.5),
        static_cast<int>(color.blue() * 0.5),
        static_cast<int>(color.alpha() * 0.5)
    ));
    paint.setPen(pen);
    paint.drawEllipse(2, 0, size - 5, size - 5);
    return pixmap;
}

/*
    формирование строки размера файла
*/
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

bool SetCurrentZipFileName(QuaZip* zip, const QString& name)
{
    bool result = zip->setCurrentFile(name, QuaZip::csInsensitive);
    if (!result) {
        zip->setFileNameCodec(QTextCodec::codecForName("IBM 866"));
        result = zip->setCurrentFile(name, QuaZip::csInsensitive);
    }
    return result;
}

QString RelativeToAbsolutePath(QString path)
{
    if (QDir(path).isRelative() && path.indexOf("%") < 0) {
        return app->applicationDirPath() + "/" + path;
    }
    return path;
}

void ResetToDefaultSettings()
{
    int size = 100;
    int id_tag = 0;

    foreach(tag i, tag_list) {
        if (i.font_name == "dropcaps_font") {
            size = i.font_size;
            break;
        }
        id_tag++;
    }

    QSettings* settings = GetSettings();
    settings->clear();
    settings->sync();
    settings->beginWriteArray("export");

    settings->setArrayIndex(0);
    settings->setValue("ExportName", QApplication::tr("Save as") + " ...");
    settings->setValue("OutputFormat", "-");
    settings->beginWriteArray("fonts");
    settings->setArrayIndex(0);
    settings->setValue("use", true);
    settings->setValue("tag", id_tag);
    settings->setValue("font", default_dropcaps_font);
    settings->setValue("fontSize", size);
    settings->endArray();

    settings->setArrayIndex(1);
    settings->setValue("ExportName", QApplication::tr("Save as") + " MOBI");
    settings->setValue("OutputFormat", "MOBI");
    settings->beginWriteArray("fonts");
    settings->setArrayIndex(0);
    settings->setValue("use", true);
    settings->setValue("tag", id_tag);
    settings->setValue("font", default_dropcaps_font);
    settings->setValue("fontSize", size);
    settings->endArray();

    settings->setArrayIndex(2);
    settings->setValue("ExportName", QApplication::tr("Save as") + " EPUB");
    settings->setValue("OutputFormat", "EPUB");
    settings->beginWriteArray("fonts");
    settings->setArrayIndex(0);
    settings->setValue("use", true);
    settings->setValue("tag", id_tag);
    settings->setValue("font", default_dropcaps_font);
    settings->setValue("fontSize", size);
    settings->endArray();

    settings->setArrayIndex(3);
    settings->setValue("ExportName", QApplication::tr("Save as") + " AZW3");
    settings->setValue("OutputFormat", "AZW3");
    settings->beginWriteArray("fonts");
    settings->setArrayIndex(0);
    settings->setValue("use", true);
    settings->setValue("tag", id_tag);
    settings->setValue("font", default_dropcaps_font);
    settings->setValue("fontSize", size);
    settings->endArray();

    settings->endArray();
}

QString FindLocaleFile(QString locale, QString name, QString suffics)
{
    QString FileName = QApplication::applicationDirPath() + "/language/" + name + "_";
    int name_len = FileName.length();
    FileName += locale + ".qm";
    bool qm = true;
    while (!QFile(FileName).exists()) {
        if (qm) {
            FileName = FileName.left(FileName.length() - (suffics.length() + 1));
            qm = false;
        }
        else {
            int pos = FileName.indexOf("_", name_len);
            if (pos < 0)
                return "";
            FileName = FileName.left(pos) + "." + suffics;
            qm = true;
        }
    }
    return FileName;
}

void SetLocale()
{
    QSettings* settings = GetSettings();
    QString locale = settings->value("localeUI", QLocale::system().name()).toString();
    setlocale(LC_ALL, "rus");
    QLocale::setDefault(QLocale(QLocale::Russian, QLocale::RussianFederation));

    QTextCodec::setCodecForLocale(QTextCodec::codecForName("UTF-8"));

    if (translator) {
        app->removeTranslator(translator);
    }
    if (translator_qt) {
        app->removeTranslator(translator_qt);
    }

    translator = new QTranslator(app);
    if (translator->load(FindLocaleFile(locale, "language", "qm"))) {
        app->installTranslator(translator);
    }
    else {
        delete translator;
        translator = nullptr;
    }

    translator_qt = new QTranslator(app);
    if (translator_qt->load(FindLocaleFile(locale, "qtbase", "qm"))) {
        app->installTranslator(translator_qt);
    }
    else {
        delete translator_qt;
        translator_qt = nullptr;
    }
    settings->setValue("localeUI", locale);
    settings->sync();

    tag_list.clear();
    tag_list <<
        tag(app->translate("SettingsDlg", "Top level captions"), ".h0", "top_caption_font", 140) <<
        tag(app->translate("SettingsDlg", "Captions"), ".h1,.h2,.h3,.h4,.h5,.h6", "caption_font", 120) <<
        tag(app->translate("SettingsDlg", "Dropcaps"), "span.dropcaps", "dropcaps_font", 300) <<
        tag(app->translate("SettingsDlg", "Footnotes"), ".inlinenote,.blocknote", "footnotes_font", 80) <<
        tag(app->translate("SettingsDlg", "Annotation"), ".annotation", "annotation_font", 100) <<
        tag(app->translate("SettingsDlg", "Poems"), ".poem", "poem_font", 100) <<
        tag(app->translate("SettingsDlg", "Epigraph"), ".epigraph", "epigraph_font", 100) <<
        tag(app->translate("SettingsDlg", "Book"), "body", "body_font", 100);
}

SendType SetCurrentExportSettings(int index)
{
    QSettings* settings = GetSettings(true);
    int count = settings->beginReadArray("export");
    QMap<QString, QVariant> map;
    QStringList keys;
    if (index >= 0 && index < count) {
        settings->setArrayIndex(index);
        keys = settings->allKeys();
        foreach(QString key, keys) {
            map.insert(key, settings->value(key));
        }
    }
    settings->endArray();
    foreach(QString key, keys) {
        settings->setValue(key, map.value(key, settings->value(key)));
    }
    settings->sync();
    delete settings;
    if (map.contains("sendTo")) {
        return (map.value("sendTo").toString() == "device" ? ST_Device : ST_Mail);
    }
    return ST_Device;
}

QString Transliteration(QString str)
{
    str = str.trimmed();
    QString fn;
    int i, rU, rL;
    QString validChars = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz1234567890-_,.()[]{}<>!@#$%^&+=\\/";
    QString rusUpper = QString::fromUtf8("АБВГДЕЁЖЗИЙКЛМНОПРСТУФХЦЧШЩЫЭЮЯ");
    QString rusLower = QString::fromUtf8("абвгдеёжзийклмнопрстуфхцчшщыэюя");
    QStringList latUpper, latLower;
    latUpper << "A" << "B" << "V" << "G" << "D" << "E" << "Jo" << "Zh" << "Z" << "I" << "J" << "K" << "L" << "M" << "N"
        << "O" << "P" << "R" << "S" << "T" << "U" << "F" << "H" << "C" << "Ch" << "Sh" << "Sh" << "I" << "E" << "Ju" << "Ja";
    latLower << "a" << "b" << "v" << "g" << "d" << "e" << "jo" << "zh" << "z" << "i" << "j" << "k" << "l" << "m" << "n"
        << "o" << "p" << "r" << "s" << "t" << "u" << "f" << "h" << "c" << "ch" << "sh" << "sh" << "i" << "e" << "ju" << "ja";
    for (i = 0; i < str.size(); ++i) {
        if (validChars.contains(str[i])) {
            fn = fn + str[i];
        }
        else if (str[i] == ' ') {  //replace spaces
            fn = fn + " ";
        }
        else if (str[i] == '?') {  //replace ?
            fn = fn + ".";
        }
        else if (str[i] == '*') {  //replace *
            fn = fn + ".";
        }
        else if (str[i] == '~') {  //replace ~
            fn = fn + ".";
        }
        else {
            rU = rusUpper.indexOf(str[i]);
            rL = rusLower.indexOf(str[i]);
            if (rU >= 0)         fn = fn + latUpper[rU];
            else if (rL >= 0)   fn = fn + latLower[rL];
        }
    }
    if (fn.isEmpty()) fn = "file";
    return fn;
}

QString ValidateFileName(QString str)
{
    bool windows = false;
    bool mac = false;
#ifdef WIN32
    windows = true;
#endif
#ifdef Q_OS_MAC
    mac = true;
#endif

    QSettings* settings = GetSettings();
    if (!settings->value("extended_symbols", false).toBool() || windows) {

        str = str.replace("\"", "'");
        str = str.replace(QRegExp("^([a-zA-Z]\\:|\\\\\\\\[^\\/\\\\:*?\"<>|]+\\\\[^\\/\\\\:*?\"<>|]+)(\\\\[^\\/\\\\:*?\"<>|]+)+(\\.[^\\/\\\\:*?\"<>|]+)$"), "_");
        str = str.left(2) + str.mid(2).replace(":", "_");
    }
    else {
        if (mac)
            str = str.replace(QRegExp("[:]"), "_");
    }
    str = str.replace(QRegExp("[/\\]"), "_");
    qDebug() << str;
    return str;
}

QString BuildFileName(QString filename)
{
    return filename.replace("/", ".").replace("\\", ".").replace("*", ".").replace("|", ".").replace(":", ".").replace("?", ".").replace("<", ".").replace(">", ".").replace("\"", "'");
}

QString decodeStr(const QString& str)
{
    if (str.left(3) != "#-#")
        return str;
    QByteArray arr = QByteArray::fromBase64(str.mid(3).toLatin1());
    quint32 index = 0;
    for (int i = 0; i < arr.size(); i++)
    {
        arr[i] = arr[i] ^ key[index];
        index++;
        if (index >= sizeof(key) / sizeof(quint32))
            index = 0;
    }
    return QString::fromUtf8(arr);
}

QString encodeStr(const QString& str)
{
    QByteArray arr(str.toUtf8());
    quint32 index = 0;
    for (int i = 0; i < arr.size(); i++)
    {
        arr[i] = arr[i] ^ key[index];
        index++;
        if (index >= sizeof(key) / sizeof(quint32))
            index = 0;
    }

    return "#-#" + QString::fromLatin1(arr.toBase64());
}

QStringList fillParams(QStringList str, book_info& bi, QFileInfo book_file)
{
    QStringList result = str;
    QString abbr = "";
    foreach(QString str, bi.seria.split(" ")) {
        abbr += str.left(1);
    }
    result.replaceInStrings("%abbrs", abbr.toLower());

    result.replaceInStrings("%fn", book_file.completeBaseName()).
        replaceInStrings("%d", book_file.absoluteDir().path()).
        replaceInStrings("%app_dir", QApplication::applicationDirPath() + "/");
    result.removeOne("%no_point");

    qDebug() << bi.authors[0].firstname;
    qDebug() << bi.authors[0].author;
    qDebug() << str;
    result.replaceInStrings("%fi", bi.authors[0].firstname.left(1) + (bi.authors[0].firstname.isEmpty() ? "" : ".")).
        replaceInStrings("%mi", bi.authors[0].middlename.left(1) + (bi.authors[0].middlename.isEmpty() ? "" : ".")).
        replaceInStrings("%li", bi.authors[0].lastname.left(1) + (bi.authors[0].lastname.isEmpty() ? UnknownAuthor : ".")).
        replaceInStrings("%nf", bi.authors[0].firstname.trimmed()).
        replaceInStrings("%nm", bi.authors[0].middlename.trimmed()).
        replaceInStrings("%nl", bi.authors[0].lastname.trimmed());

    result.replaceInStrings("%f", book_file.absoluteFilePath());

    result.replaceInStrings("%s", bi.seria.trimmed()).replaceInStrings("%b", bi.title.trimmed()).replaceInStrings("%a", bi.authors[0].author.replace(",", " ").trimmed());
    QString num_in_seria = QString::number(bi.num_in_seria);
    for (int i = 0; i < result.count(); i++) {
        if (result[i].contains("%n")) {
            int len = result[i].mid(result[i].indexOf("%n") + 2, 1).toInt();
            QString zerro;
            if (bi.num_in_seria == 0)
                result[i].replace("%n" + QString::number(len), "");
            else
                result[i].replace("%n" + (len > 0 ? QString::number(len) : ""), (len > 0 ? zerro.fill('0', len - num_in_seria.length()) : "") + num_in_seria + " ");
        }
        result[i] = result[i].trimmed();
    }
    result.replaceInStrings("/ ", "/");
    result.replaceInStrings("/.", "/");
    result.replaceInStrings("////", "/");
    result.replaceInStrings("///", "/");
    result.replaceInStrings("//", "/");
    return result;
}

QString fillParams(QString str, book_info& bi, QFileInfo book_file)
{
    QStringList result = fillParams(QStringList() << str, bi, book_file);
    return result[0];
}

QStringList fillParams(QStringList str, QFileInfo book_file, QString seria_name, QString book_name, QString author, QString ser_num)
{
    book_info bi;
    bi.seria = seria_name;
    bi.title = book_name;
    bi.authors << author_info(author, -1);
    bi.num_in_seria = ser_num.toInt();
    return fillParams(str, bi, book_file);
}

QString fillParams(QString str, QFileInfo book_file, QString seria_name, QString book_name, QString author, QString ser_num)
{
    QStringList result = fillParams(QStringList() << str, book_file, seria_name, book_name, author, ser_num);
    return result[0];
}


void myMessageOutput(QtMsgType type, const QMessageLogContext& context, const QString& msg)
{
    QString tmp_dir;
    if (QStandardPaths::standardLocations(QStandardPaths::TempLocation).count() > 0)
        tmp_dir = QStandardPaths::standardLocations(QStandardPaths::TempLocation).at(0);
    tmp_dir += "/freeLib/log.txt";
    QFile file(tmp_dir);
    file.open(QIODevice::Append | QIODevice::WriteOnly);
    QByteArray localMsg = msg.toLocal8Bit();
    switch (type) {
    case QtDebugMsg:
        file.write(localMsg.constData());
        file.write("\r\n");
        fprintf(stderr, "Debug: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtWarningMsg:
        fprintf(stderr, "Warning: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtCriticalMsg:
        fprintf(stderr, "Critical: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        break;
    case QtFatalMsg:
        fprintf(stderr, "Fatal: %s (%s:%u, %s)\n", localMsg.constData(), context.file, context.line, context.function);
        abort();
    default:
        break;
    }
}

void GetBookInfo(book_info& bi, const QByteArray& data, QString type,
    bool& isBookWithoutTitle, bool& isAuthorWithoutData, bool& isSeriaWithoutName, bool& isGenreaWithoutName,
    bool info_only, uint id_book)
{
    QSettings settings;
    bi.id = id_book;
    if (id_book == 0 || !info_only) {
        if (type == "epub") {
            QBuffer buf;
            buf.setData(data);
            QuaZip zip(&buf);
            zip.open(QuaZip::mdUnzip);
            QBuffer info;
            SetCurrentZipFileName(&zip, "META-INF/container.xml");
            QuaZipFile zip_file(&zip);
            zip_file.open(QIODevice::ReadOnly);
            info.setData(zip_file.readAll());
            zip_file.close();
            QDomDocument doc;
            doc.setContent(info.data());
            QDomNode root = doc.documentElement();
            bool need_loop = true;
            QString rel_path;
            bi.num_in_seria = 0;
            for (int i = 0; i < root.childNodes().count() && need_loop; i++) {
                if (root.childNodes().at(i).nodeName().toLower() == "rootfiles") {
                    QDomNode roots = root.childNodes().at(i);
                    for (int j = 0; j < roots.childNodes().count() && need_loop; j++) {
                        if (roots.childNodes().at(j).nodeName().toLower() == "rootfile") {
                            QString path = roots.childNodes().at(j).attributes().namedItem("full-path").toAttr().value();
                            QBuffer opf_buf;
                            QFileInfo fi(path);
                            rel_path = fi.path();
                            SetCurrentZipFileName(&zip, path);
                            zip_file.open(QIODevice::ReadOnly);
                            opf_buf.setData(zip_file.readAll());
                            zip_file.close();

                            QDomDocument opf;
                            opf.setContent(opf_buf.data());
                            QDomNode meta = opf.documentElement().namedItem("metadata");
                            for (int m = 0; m < meta.childNodes().count(); m++) {
                                if (meta.childNodes().at(m).nodeName().right(5) == "title") {
                                    bi.title = meta.childNodes().at(m).toElement().text().trimmed();
                                }
                                else if (meta.childNodes().at(m).nodeName().right(8) == "language") {
                                    bi.language = meta.childNodes().at(m).toElement().text().trimmed();
                                }
                                else if (meta.childNodes().at(m).nodeName().right(7) == "creator") {
                                    QStringList names = meta.childNodes().at(m).toElement().text().trimmed().split(" ");
                                    names.move(names.count() - 1, 0);
                                    if (names.count() == 3)
                                        names.append(""); // NickName
                                    bi.authors << author_info(names, 0);
                                }
                                else if (meta.childNodes().at(m).nodeName().right(7) == "subject") {
                                    QString  genre = meta.childNodes().at(m).toElement().text().trimmed();
                                    if (!genre.isEmpty())
                                        bi.genres << genre_info(meta.childNodes().at(m).toElement().text().trimmed(), 0);
                                }
                                else if (meta.childNodes().at(m).nodeName().right(11) == "description") {
                                    QBuffer buff;
                                    buff.open(QIODevice::WriteOnly);
                                    QTextStream ts(&buff);
                                    ts.setCodec("UTF-8");
                                    meta.childNodes().at(m).save(ts, 0, QDomNode::EncodingFromTextStream);
                                    bi.annotation = QString::fromUtf8(buff.data().data());
                                }
                                else if (meta.childNodes().at(m).nodeName().right(8) == "coverage") {
                                    QString coverage = meta.childNodes().at(m).toElement().text().trimmed();
                                    if (!coverage.isEmpty())
                                        bi.keywords += !bi.keywords.isEmpty() ? (" , " + coverage) : coverage;
                                }
                                else if (meta.childNodes().at(m).nodeName().right(4) == "meta" && info_only) {
                                    QString metaName = meta.childNodes().at(m).attributes().namedItem("name").toAttr().value();
                                    if (metaName == "calibre:title_sort") {
                                        if (bi.title == "")
                                            bi.title = meta.childNodes().at(m).attributes().namedItem("content").toAttr().value().trimmed();
                                    }
                                    else if (metaName == "calibre:series")
                                        bi.seria = meta.childNodes().at(m).attributes().namedItem("content").toAttr().value().trimmed();
                                    else if (metaName == "calibre:series_index")
                                        bi.num_in_seria = meta.childNodes().at(m).attributes().namedItem("content").toAttr().value().trimmed().toInt();
                                    else if (metaName == "keywords" || metaName == "keyword" || metaName == "keys") {
                                        QString keywords = meta.childNodes().at(m).attributes().namedItem("content").toAttr().value().trimmed();
                                        if (!keywords.isEmpty())
                                            bi.keywords += !bi.keywords.isEmpty() ? (" , " + keywords) : keywords;
                                    }
                                }
                                else if (meta.childNodes().at(m).nodeName().right(4) == "meta" && !info_only) {
                                    if (meta.childNodes().at(m).attributes().namedItem("name").toAttr().value() == "cover") {
                                        QString cover = meta.childNodes().at(m).attributes().namedItem("content").toAttr().value();
                                        QDomNode manifest = opf.documentElement().namedItem("manifest");
                                        for (int man = 0; man < manifest.childNodes().count(); man++) {
                                            if (manifest.childNodes().at(man).attributes().namedItem("id").toAttr().value() == cover) {
                                                QBuffer img;
                                                cover = rel_path + "/" + manifest.childNodes().at(man).attributes().namedItem("href").toAttr().value();

                                                SetCurrentZipFileName(&zip, cover);
                                                zip_file.open(QIODevice::ReadOnly);
                                                img.setData(zip_file.readAll());
                                                zip_file.close();

                                                //проверить как работает
                                                QString sImgFile = QString("%1/freeLib/cover.jpg").arg(QStandardPaths::standardLocations(QStandardPaths::TempLocation).constFirst());
                                                QPixmap image;
                                                image.loadFromData(img.data());
                                                // масштабирование обложки книги, если необходимо
                                                QSize picSize = GetCoverSize();
                                                if (picSize.width() != -1 && picSize.height() != -1)
                                                    image = image.scaled(picSize, Qt::KeepAspectRatio);
                                                image.save(sImgFile);

                                                //bi.img=("<td valign=top style=\"width:%1px\"><center><img src=\"data:"+manifest.childNodes().at(man).attributes().namedItem("media-type").toAttr().value()+
                                                //        ";base64,"+img.data().toBase64()+"\"></center></td>");
                                                bi.img = QString("<td valign=top style=\"width:1px\"><center><img src=\"file:%1\"></center></td>").arg(sImgFile);
                                                break;
                                            }
                                        }
                                    }
                                }
                            }
                            need_loop = false;
                        }
                    }
                }
            }
            zip.close();
            buf.close();
            info.close();
        }
        else if (type == "fb2" || type == "fbd") {
            QDomDocument doc;
            doc.setContent(data);
            QDomElement title_info = doc.elementsByTagName("title-info").at(0).toElement();
            if (!info_only) {
                QString cover = QString::fromStdString(title_info.elementsByTagName("coverpage").at(0).toElement().elementsByTagName("image").at(0).attributes().namedItem("l:href").toAttr().value().toStdString());
                if (cover == "")
                    cover = QString::fromStdString(title_info.elementsByTagName("coverpage").at(0).toElement().elementsByTagName("image").at(0).attributes().namedItem("xlink:href").toAttr().value().toStdString());
                if (cover.left(1) == "#") {
                    QDomNodeList binarys = doc.elementsByTagName("binary");
                    for (int i = 0; i < binarys.count(); i++) {
                        if (binarys.at(i).attributes().namedItem("id").toAttr().value() == cover.right(cover.length() - 1)) {
                            QString sImgFile = QString("%1/freeLib/cover.jpg").arg(QStandardPaths::standardLocations(QStandardPaths::TempLocation).constFirst());
                            QPixmap image;
                            QByteArray ba;
                            ba.append(binarys.at(i).toElement().text());
                            QByteArray ba64 = QByteArray::fromBase64(ba);
                            image.loadFromData(ba64);

                            // масштабирование обложки книги, если необходимо
                            QSize picSize = GetCoverSize();
                            if (picSize.width() != -1 && picSize.height() != -1)
                                image = image.scaled(picSize, Qt::KeepAspectRatio);

                            image.save(sImgFile);
                            bi.img = QString("<td valign=top><center><img src=\"file:%1\"></center></td>").arg(sImgFile);
                            break;
                        }
                    }
                }
                QBuffer buff;
                buff.open(QIODevice::WriteOnly);
                QTextStream ts(&buff);
                ts.setCodec("UTF-8");
                title_info.elementsByTagName("annotation").at(0).save(ts, 0, QDomNode::EncodingFromTextStream);
                bi.annotation = QString::fromUtf8(buff.data().data());
                bi.annotation.replace("<annotation>", "", Qt::CaseInsensitive);
                bi.annotation.replace("</annotation>", "", Qt::CaseInsensitive);
            }
            bi.title = title_info.elementsByTagName("book-title").at(0).toElement().text().trimmed();
            bi.keywords = title_info.elementsByTagName("keywords").at(0).toElement().text().trimmed();
            bi.language = title_info.elementsByTagName("lang").at(0).toElement().text();
            bi.seria = title_info.elementsByTagName("sequence").at(0).attributes().namedItem("name").toAttr().value().trimmed();
            bi.num_in_seria = title_info.elementsByTagName("sequence").at(0).attributes().namedItem("number").toAttr().value().trimmed().toInt();

            QDomNodeList serias = title_info.elementsByTagName("sequence");
            for (int i = 0; i < serias.count(); i++) {
                QString s = serias.at(i).attributes().namedItem("name").toAttr().value().trimmed();
                int n = serias.at(i).attributes().namedItem("number").toAttr().value().trimmed().toInt();
                if (!s.isEmpty())
                    bi.serias << seria_info(0, s, n);
            }

            QDomNodeList author = title_info.elementsByTagName("author");
            for (int i = 0; i < author.count(); i++) {
                author_info ti(UnknownAuthor, 0);
                ti.id = 0;
                ti.firstname = author.at(i).toElement().elementsByTagName("first-name").at(0).toElement().text();
                ti.lastname = author.at(i).toElement().elementsByTagName("last-name").at(0).toElement().text();
                ti.middlename = author.at(i).toElement().elementsByTagName("middle-name").at(0).toElement().text();
                ti.nickname = author.at(i).toElement().elementsByTagName("nickname").at(0).toElement().text();
                ti.author = ti.lastname + "," + ti.firstname + "," + ti.middlename + "," + ti.nickname;
                if (ti.firstname.isEmpty() && ti.lastname.isEmpty() && ti.middlename.isEmpty() && ti.nickname.isEmpty()) {
                    ti.lastname = ti.author = UnknownAuthor;
                    isAuthorWithoutData = true;
                }
                bi.authors << ti;
            }

            QDomNodeList genre = title_info.elementsByTagName("genre");
            for (int i = 0; i < genre.count(); i++) {
                QString g = genre.at(i).toElement().text().trimmed();
                if (!g.isEmpty())
                    bi.genres << genre_info(genre.at(i).toElement().text().trimmed(), 0);
            }

            QDomElement publish_info = doc.elementsByTagName("publish-info").at(0).toElement();
            bi.isbn = publish_info.elementsByTagName("isbn").at(0).toElement().text();
        }
    }
    if (id_book > 0) {
        bi.authors.clear();
        bi.title = mLibs[g_idCurrentLib].mBooks[id_book].sName;
        bi.num_in_seria = mLibs[g_idCurrentLib].mBooks[id_book].numInSerial;
        bi.language = mLibs[g_idCurrentLib].vLaguages[mLibs[g_idCurrentLib].mBooks[id_book].idLanguage];
        bi.seria = mLibs[g_idCurrentLib].mSerials[mLibs[g_idCurrentLib].mBooks[id_book].idSerial].sName;
        bi.id_seria = mLibs[g_idCurrentLib].mBooks[id_book].idSerial;
        bi.readed = mLibs[g_idCurrentLib].mBooks[id_book].bReaded;
        bi.keywords = mLibs[g_idCurrentLib].mBooks[id_book].sKeywords;

        foreach(uint idAuthor, mLibs[g_idCurrentLib].mBooks[id_book].listIdAuthors) {
            author_info ti("", 0);
            ti.id = idAuthor;
            ti.author = mLibs[g_idCurrentLib].mAuthors[idAuthor].getName();
            bi.authors << ti;
        }

        bi.genres.clear();
        foreach(uint idGenre, mLibs[g_idCurrentLib].mBooks[id_book].listIdGenres) {
            bi.genres << genre_info(mGenre[idGenre].sName.trimmed(), idGenre);
        }

    }

    if (bi.title.isEmpty()) {
        isBookWithoutTitle = true;
        bi.title = WithoutTitle;
    }
    if (bi.genres.count() == 0) {
        isGenreaWithoutName = true;
        bi.genres << genre_info(WithoutGenre, 0);
    }
    if (bi.authors.count() == 0) {
        isAuthorWithoutData = true;
        bi.authors << author_info(UnknownAuthor, 0);
    }
    if (bi.language.isEmpty())
        bi.language = "ru";
    if (bi.seria.isEmpty()) {
        isSeriaWithoutName = true;
        bi.seria = WithoutSeries;
    }
}

/*
    Масштаб обложки книги из настроек
*/
QSize GetCoverSize()
{
    QSettings settings;
    QSize picSize;
    switch (settings.value("CoverSize", 0).toInt()) {
    case 0:
        picSize.setWidth(90);
        picSize.setHeight(120);
        break;
    case 1:
        picSize.setWidth(120);
        picSize.setHeight(160);
        break;
    case 2:
        picSize.setWidth(150);
        picSize.setHeight(200);
        break;
    case 3:
        picSize.setWidth(240);
        picSize.setHeight(320);
        break;
    case 4:
        picSize.setWidth(300);
        picSize.setHeight(400);
        break;
    case 5:
        picSize.setWidth(480);
        picSize.setHeight(640);
        break;
    case 6:
        picSize.setWidth(600);
        picSize.setHeight(800);
        break;
    case 7:
        return QSize();
        break;
    default:
        picSize.setWidth(300);
        picSize.setHeight(400);
        break;
    }
    return picSize;
}
