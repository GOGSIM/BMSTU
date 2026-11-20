#include "MainWindow.h"
#include <QApplication>
#include <QLibraryInfo>
#include <QTranslator>

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QTranslator translator;
    if (translator.load("qt_ru", QLibraryInfo::path(QLibraryInfo::TranslationsPath)))
        app.installTranslator(&translator);
    MainWindow window;
    window.show();
    for (int i = 1; i < app.arguments().size(); ++i) window.openFile(app.arguments().at(i));
    return app.exec();
}
