#include "MainWindow.h"
#include <QApplication>

int main(int argc, char* argv[])
{
    // QApplication создаётся раньше виджетов и обслуживает цикл событий Qt.
    QApplication application(argc, argv);
    MainWindow window;
    window.show();
    // Необязательные аргументы — пути к XML для быстрого открытия при запуске.
    const QStringList files = application.arguments().mid(1);
    for (const QString& file : files) {
        window.openFile(file);
    }
    // exec() передаёт события окнам, а сигналы вызывают связанные слоты.
    return application.exec();
}
