// Стандартные библиотеки и объявления двух частей программы.
#include <iostream>
#include <limits>
#include <ctime>
#include <cstdlib>
#include "SequentialPart.h"
#include "Warehouse.h"

// Безымянное пространство имён скрывает вспомогательную функцию внутри файла.
namespace
{
// Читает целый номер пункта и не завершает программу при ошибочном вводе.
int readMenuChoice()
{
    int choice = 0;

    // Оператор >> возвращает поток. Знак ! проверяет, произошла ли ошибка.
    while (!(std::cin >> choice))
    {
        // eof сообщает, что данные закончились, например был закрыт stdin.
        if (std::cin.eof())
        {
            return 0;
        }

        // clear снимает флаг ошибки, а ignore удаляет неверную строку.
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Incorrect input. Enter an integer: ";
    }

    return choice;
}

}

// main — точка входа в программу.
int main()
{
    // static_cast явно преобразует время в тип, который принимает srand.
    // nullptr означает отсутствие указателя: функции time не нужен адрес результата.
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

    int choice = -1;

    // Меню повторяется, пока пользователь не выбрал ноль.
    while (choice != 0)
    {
        std::cout << "\nHomework 1. Input/output streams and STL\n"
                  << "1. Sequential containers\n"
                  << "2. Associative container\n"
                  << "0. Exit\n"
                  << "Choice: ";

        choice = readMenuChoice();

        // switch выбирает действие по введённому номеру.
        switch (choice)
        {
        case 1:
            sequential::runMenu();
            break;
        case 2:
            warehouse::runMenu();
            break;
        case 0:
            break;
        default:
            std::cout << "Unknown menu item.\n";
        }
    }

    return 0; // Ноль сообщает операционной системе об успешном завершении.
}
