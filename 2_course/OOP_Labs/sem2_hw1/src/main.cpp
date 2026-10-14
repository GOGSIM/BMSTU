#include <iostream>
#include <limits>
#include <ctime>
#include <cstdlib>
#include "SequentialPart.h"
#include "Warehouse.h"

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

}

int main()
{
    std::srand(static_cast<unsigned int>(std::time(nullptr)));

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

    return 0;
}
