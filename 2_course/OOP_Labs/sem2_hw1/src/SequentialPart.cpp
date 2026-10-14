#include "SequentialPart.h"

#include <algorithm>
#include <cstdlib>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <limits>
#include <stdexcept>
#include <vector>

namespace
{
const int RANDOM_NUMBER_COUNT = 100;
const int MIN_RANDOM_VALUE = -50;
const int RANDOM_VALUE_COUNT = 101;

int generateRandomNumber()
{
    return MIN_RANDOM_VALUE + std::rand() % RANDOM_VALUE_COUNT;
}

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

namespace sequential
{
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

void createDataWithAlgorithms(const std::string& fileName)
{
    std::vector<int> data(RANDOM_NUMBER_COUNT);
    std::generate(data.begin(), data.end(), generateRandomNumber);

    std::ofstream output(fileName);

    if (!output)
    {
        throw std::runtime_error("Cannot open the input data file for writing.");
    }

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

void modifyWithTransform(
    const std::string& inputFileName,
    const std::string& outputFileName
)
{
    const std::vector<double> source =
        loadDataWithIterators<std::vector<double>>(inputFileName);
    const double divisor = detail::calculateDivisor(
        source.cbegin(),
        source.cend()
    );

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

    std::transform(
        std::istream_iterator<double>(input),
        std::istream_iterator<double>(),
        std::ostream_iterator<double>(output, "\n"),
        [divisor, &absoluteSum, &elementCount](double value)
        {
            const double result = value / divisor;
            absoluteSum += std::abs(result);
            ++elementCount;
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

    output << absoluteSum << '\n'
           << absoluteSum / static_cast<double>(elementCount) << '\n';

    if (!output)
    {
        throw std::runtime_error("Cannot write all transformed data to the file.");
    }
}

void runMenu()
{
    int choice = -1;

    while (choice != 0)
    {
        std::cout << "\nSequential containers\n"
                  << "1. Create input.txt with file streams\n"
                  << "2. Create input.txt with generate and copy\n"
                  << "0. Return\n"
                  << "Choice: ";

        choice = readMenuChoice();

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
            case 0:
                break;
            default:
                std::cout << "Unknown menu item.\n";
            }
        }
        catch (const std::exception& error)
        {
            std::cout << "Error: " << error.what() << '\n';
        }
    }
}
}

