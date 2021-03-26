# freeLib
freeLib - каталогизатор для библиотек LibRusEc, Flibusta, CoolLib...
Одновременно в библиотеку можно добавлять fb2, epub книги, а также книги любых форматов (хоть аудиокниги), для которых есть описание их данных в одноименном файле формата fbd. Причем книга и ее fbd описание могут располагаться рядом, могут быть сжаты в zip архив, могут размещаться в папке, которая сжата в zip архив.

**petrovvlad**: Это форк общедоступного freeLib 5.0 , разработка которого прекращена. 
Сборка протестирована в Qt 5.11.0 (Linux).

**dikbsd**: 
1. Код адаптирован для Windows (для отображения данных книг вместо QWebEngineView используется QTextBrowser). Сборка протестирована на Windows 10 (x64) в Visual Studio 2019 с Qt 5.15.2 (msvc2019_64) и Windows (x86) в Visual Studio 2019 с Qt 5.15.2 (msvc2019).

2. Адаптация кода под Linux. Сборка протестирована на Linux Mint 20.1 x64 в QtCreator с Qt 5.12.10.

## Сборка кода под Windows
1. Загрузите и установите Qt фреймворк по ссылке: https://www.qt.io/download-qt-installer

2. Загрузите и установите аддон по работе с Qt в MS Visual Studio 2019:
https://marketplace.visualstudio.com/items?itemName=TheQtCompany.QtVisualStudioTools2019

3. Настройте Qt в MS Visual Studio 2019:

- В MS Visual Studio 2019 в меню Расширения > Qt VS Tools выберите Qt Versions.
- В диалоге, в таблице версий Qt:
  -- Для x64 в поле Path укажите путь к Qt: C:\Qt\5.15.2\msvc2019_64. В поле Version измените название версии на msvc2019_64.
  -- Для x86 в поле Path укажите путь к Qt: C:\Qt\5.15.2\msvc2019. В поле Version измените название версии на msvc2019.
- Если у Вас основная сборка будет под x64, то в поле Default поставьте галочку для msvc2019_64.
- Если у Вас основная сборка будет под x86, то в поле Default поставьте галочку для msvc2019.

4. Загрузите код библиотекаря:
```
git clone --recurse-submodules https://github.com/petrovvlad/freeLib.git
```

5. Произведите сборку кода в MS Visual Studio 2019, открыв файл Lib_win.sln.

## Сбока кода под Linux
1. Предварительная установка в систему необходимых библиотек:

```
sudo apt install build-essential libquazip*
sudo apt install qt5-default qttools5-dev-tools qttools5-dev qtwebengine5-dev
```

2. Загрузите с сайта https://www.qt.io/offline-installers пакет `qt-opensource-linux-x64-5.12.10.run`  или более новый.

3. Задайте инсталятору Qt права на выполнение:
```
chmod a+x qt-opensource-linux-x64-5.12.10.run
```

4. Запустите инсталятор:
```
$ ./qt-opensource-linux-x64-5.12.10.run
```
- Выберите для установки `Desctop Qt 5.12.10 GCC 64 bit`.

5. Загрузите код библиотекаря в нужную папку:
```
git clone --recurse-submodules https://github.com/DikBSD/freeLib.git
```

6. Произведите сборку кода в QtCreator, открыв файл Lib_linux.pro.
