#include <iostream>
#include <limits>

namespace
{
int readMenuChoice()
{
    int choice = 0;

    while (!(std::cin >> choice))
    {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Incorrect input. Enter an integer: ";
    }

    return choice;
}

void runSequentialMenu()
{
    std::cout << "Sequential containers part is not implemented yet.\n";
}

void runWarehouseMenu()
{
    std::cout << "Associative container part is not implemented yet.\n";
}
}

int main()
{
    int choice = -1;

    while (choice != 0)
    {
        std::cout << "\nHomework 1. Input/output streams and STL\n"
                  << "1. Sequential containers\n"
                  << "2. Associative container\n"
                  << "0. Exit\n"
                  << "Choice: ";

        choice = readMenuChoice();

        switch (choice)
        {
        case 1:
            runSequentialMenu();
            break;
        case 2:
            runWarehouseMenu();
            break;
        case 0:
            break;
        default:
            std::cout << "Unknown menu item.\n";
        }
    }

    return 0;
}
