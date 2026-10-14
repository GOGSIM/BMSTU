// Объявления первой части и необходимые стандартные библиотеки.
#include "SequentialPart.h"

#include <algorithm>
#include <cstdlib>
#include <deque>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <list>
#include <limits>
#include <stdexcept>
#include <vector>

// Безымянное пространство имён скрывает служебные функции внутри файла.
namespace
{
// Количество и границы генерируемых значений.
const int RANDOM_NUMBER_COUNT = 100;
const int MIN_RANDOM_VALUE = -50;
// В диапазоне от -50 до 50 находится 101 целое число.
const int RANDOM_VALUE_COUNT = 101;

// Возвращает одно случайное целое число из диапазона [-50; 50].
int generateRandomNumber()
{
    // Остаток от деления ограничивает rand нужным количеством значений.
    return MIN_RANDOM_VALUE + std::rand() % RANDOM_VALUE_COUNT;
}

// Читает номер пункта меню и очищает поток после неверного ввода.
int readMenuChoice()
{
    int choice = 0;

    while (!(std::cin >> choice))
    {
        if (std::cin.eof())
        {
            return 0;
        }

        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "Incorrect input. Enter an integer: ";
    }

    return choice;
}

// Спрашивает способ загрузки и возвращает заполненный контейнер.
template <typename Container>
Container selectLoadMethod()
{
    std::cout << "1. Load with file streams\n"
              << "2. Load with stream iterators and copy\n"
              << "Choice: ";

    const int choice = readMenuChoice();

    if (choice == 1)
    {
        return sequential::loadDataWithStreams<Container>(
            sequential::INPUT_FILE_NAME
        );
    }

    if (choice == 2)
    {
        return sequential::loadDataWithIterators<Container>(
            sequential::INPUT_FILE_NAME
        );
    }

    throw std::runtime_error("Unknown load method.");
}

// Спрашивает одну из трёх контейнерных версий modify.
template <typename Container>
Container selectModifyMethod(const Container& source)
{
    std::cout << "1. Modify the container\n"
              << "2. Modify with iterator arguments\n"
              << "3. Modify with for_each\n"
              << "Choice: ";

    const int choice = readMenuChoice();

    if (choice == 1)
    {
        return sequential::modifyContainer(source);
    }

    if (choice == 2)
    {
        Container result;
        // cbegin и cend передают функции весь диапазон исходного контейнера.
        sequential::modifyWithIterators(
            source.cbegin(),
            source.cend(),
            result
        );
        return result;
    }

    if (choice == 3)
    {
        return sequential::modifyWithForEach(source);
    }

    throw std::runtime_error("Unknown modification method.");
}

// Спрашивает способ записи готового контейнера в output.txt.
template <typename Container>
void selectOutputMethod(const Container& result)
{
    std::cout << "1. Output with file streams\n"
              << "2. Output with a stream iterator and copy\n"
              << "Choice: ";

    const int choice = readMenuChoice();

    if (choice == 1)
    {
        sequential::outputResultWithStreams(
            result,
            sequential::OUTPUT_FILE_NAME
        );
        return;
    }

    if (choice == 2)
    {
        sequential::outputResultWithIterators(
            result,
            sequential::OUTPUT_FILE_NAME
        );
        return;
    }

    throw std::runtime_error("Unknown output method.");
}

// Объединяет выбор загрузки, изменения и вывода для одного типа контейнера.
template <typename Container>
void processContainer()
{
    const Container source = selectLoadMethod<Container>();
    const Container result = selectModifyMethod(source);
    selectOutputMethod(result);

    std::cout << "The result contains " << result.size()
              << " values and is saved to output.txt.\n";
}
}

namespace sequential
{
// Создаёт input.txt обычным циклом и оператором вывода <<.
void createDataWithStreams(const std::string& fileName)
{
    std::ofstream output(fileName);

    if (!output)
    {
        throw std::runtime_error("Cannot open the input data file for writing.");
    }

    for (int index = 0; index < RANDOM_NUMBER_COUNT; ++index)
    {
        output << generateRandomNumber() << '\n';
    }

    if (!output)
    {
        throw std::runtime_error("Cannot write all generated data to the file.");
    }
}

// Создаёт input.txt с помощью generate, copy и ostream_iterator.
void createDataWithAlgorithms(const std::string& fileName)
{
    // vector сразу содержит ровно 100 элементов.
    std::vector<int> data(RANDOM_NUMBER_COUNT);
    // generate вызывает generateRandomNumber отдельно для каждого элемента.
    std::generate(data.begin(), data.end(), generateRandomNumber);

    std::ofstream output(fileName);

    if (!output)
    {
        throw std::runtime_error("Cannot open the input data file for writing.");
    }

    // ostream_iterator записывает каждый элемент в поток и добавляет перевод строки.
    std::copy(
        data.cbegin(),
        data.cend(),
        std::ostream_iterator<int>(output, "\n")
    );

    if (!output)
    {
        throw std::runtime_error("Cannot write all generated data to the file.");
    }
}

// Читает файл два раза: сначала вычисляет делитель, затем выполняет transform.
void modifyWithTransform(
    const std::string& inputFileName,
    const std::string& outputFileName
)
{
    // Первый проход загружает данные и позволяет проверить все ошибки.
    const std::vector<double> source =
        loadDataWithIterators<std::vector<double>>(inputFileName);
    const double divisor = detail::calculateDivisor(
        source.cbegin(),
        source.cend()
    );

    // Повторно открываем исходный файл для потокового преобразования.
    std::ifstream input(inputFileName);
    std::ofstream output(outputFileName);

    if (!input)
    {
        throw std::runtime_error("Cannot open the input data file.");
    }

    if (!output)
    {
        throw std::runtime_error("Cannot open the result file for writing.");
    }

    output << std::setprecision(15);

    double absoluteSum = 0.0;
    std::size_t elementCount = 0;

    // transform читает число, применяет лямбда-функцию и сразу пишет результат.
    std::transform(
        std::istream_iterator<double>(input),
        std::istream_iterator<double>(),
        std::ostream_iterator<double>(output, "\n"),
        // divisor копируется, а сумма и счётчик захватываются по ссылке через &.
        [divisor, &absoluteSum, &elementCount](double value)
        {
            const double result = value / divisor;
            absoluteSum += std::abs(result);
            ++elementCount; // Префиксный ++ увеличивает счётчик на единицу.
            return result;
        }
    );

    if (!input.eof())
    {
        throw std::runtime_error("The input data file contains an invalid value.");
    }

    if (elementCount == 0)
    {
        throw std::runtime_error("Cannot calculate statistics for empty data.");
    }

    // После 100 преобразованных чисел дописываем сумму и среднее.
    output << absoluteSum << '\n'
           << absoluteSum / static_cast<double>(elementCount) << '\n';

    if (!output)
    {
        throw std::runtime_error("Cannot write all transformed data to the file.");
    }
}

// Показывает все операции первой части и обрабатывает исключения.
void runMenu()
{
    int choice = -1;

    while (choice != 0)
    {
        std::cout << "\nSequential containers\n"
                  << "1. Create input.txt with file streams\n"
                  << "2. Create input.txt with generate and copy\n"
                  << "3. Process a vector\n"
                  << "4. Process a deque\n"
                  << "5. Process a list\n"
                  << "6. Process the file with transform and stream iterators\n"
                  << "0. Return\n"
                  << "Choice: ";

        choice = readMenuChoice();

        // try позволяет вернуть пользователя в меню после ошибки операции.
        try
        {
            switch (choice)
            {
            case 1:
                createDataWithStreams(INPUT_FILE_NAME);
                std::cout << "The file input.txt contains 100 random numbers.\n";
                break;
            case 2:
                createDataWithAlgorithms(INPUT_FILE_NAME);
                std::cout << "The file input.txt contains 100 random numbers.\n";
                break;
            case 3:
                processContainer<std::vector<double>>();
                break;
            case 4:
                processContainer<std::deque<double>>();
                break;
            case 5:
                processContainer<std::list<double>>();
                break;
            case 6:
                modifyWithTransform(INPUT_FILE_NAME, OUTPUT_FILE_NAME);
                std::cout << "The transformed data is saved to output.txt.\n";
                break;
            case 0:
                break;
            default:
                std::cout << "Unknown menu item.\n";
            }
        }
        // catch получает стандартное исключение по константной ссылке.
        catch (const std::exception& error)
        {
            std::cout << "Error: " << error.what() << '\n';
        }
    }
}
}

