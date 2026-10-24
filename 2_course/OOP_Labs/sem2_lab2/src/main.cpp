#include <QApplication>

#include "CalculatorWindow.h"

int main(int argc, char* argv[])
{
    QApplication application(argc, argv);

    CalculatorWindow window;
    window.show();

    return application.exec();
}
