#ifndef SEQUENTIAL_PART_H
#define SEQUENTIAL_PART_H

#include <algorithm>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <string>

namespace sequential
{
const std::string INPUT_FILE_NAME = "input.txt";
const std::string OUTPUT_FILE_NAME = "output.txt";

void createDataWithStreams(const std::string& fileName);
void createDataWithAlgorithms(const std::string& fileName);

template <typename Container>
Container loadDataWithStreams(const std::string& fileName)
{
    std::ifstream input(fileName);

    if (!input)
    {
        throw std::runtime_error("Cannot open the input data file.");
    }

    Container data;
    typename Container::value_type value{};

    while (input >> value)
    {
        data.push_back(value);
    }

    if (!input.eof())
    {
        throw std::runtime_error("The input data file contains an invalid value.");
    }

    return data;
}

template <typename Container>
Container loadDataWithIterators(const std::string& fileName)
{
    std::ifstream input(fileName);

    if (!input)
    {
        throw std::runtime_error("Cannot open the input data file.");
    }

    Container data;
    typedef typename Container::value_type ValueType;

    std::copy(
        std::istream_iterator<ValueType>(input),
        std::istream_iterator<ValueType>(),
        std::back_inserter(data)
    );

    if (!input.eof())
    {
        throw std::runtime_error("The input data file contains an invalid value.");
    }

    return data;
}

void runMenu();
}

#endif

