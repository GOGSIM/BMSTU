#include <QApplication>

#include "CalculatorWindow.h"

int main(int argc, char* argv[])
{
    // QApplication управляет приложением и получает аргументы командной строки.
    QApplication application(argc, argv);

    // Создаём главное окно в стеке и делаем его видимым.
    CalculatorWindow window;
    window.show();

    // exec запускает цикл событий: ожидание кликов, клавиш и закрытия окна.
    return application.exec();
}
