#ifndef SEQUENTIAL_PART_H
#define SEQUENTIAL_PART_H

#include <algorithm>
#include <cmath>
#include <fstream>
#include <iterator>
#include <numeric>
#include <stdexcept>
#include <string>

namespace sequential
{
const std::string INPUT_FILE_NAME = "input.txt";
const std::string OUTPUT_FILE_NAME = "output.txt";

namespace detail
{
template <typename Iterator>
typename std::iterator_traits<Iterator>::value_type calculateDivisor(
    Iterator begin,
    Iterator end
)
{
    typedef typename std::iterator_traits<Iterator>::value_type ValueType;

    if (std::distance(begin, end) < 50)
    {
        throw std::runtime_error("At least 50 values are required.");
    }

    Iterator firstNegative = std::find_if(
        begin,
        end,
        [](const ValueType& value)
        {
            return value < ValueType();
        }
    );

    if (firstNegative == end)
    {
        throw std::runtime_error("The data does not contain a negative value.");
    }

    Iterator fiftieth = begin;
    std::advance(fiftieth, 49);

    const ValueType divisor =
        (*firstNegative + *fiftieth) / static_cast<ValueType>(2);

    if (divisor == ValueType())
    {
        throw std::runtime_error("The division value is zero.");
    }

    return divisor;
}

template <typename Container>
void appendAbsoluteStatistics(Container& data)
{
    typedef typename Container::value_type ValueType;
    const typename Container::size_type elementCount = data.size();

    if (elementCount == 0)
    {
        throw std::runtime_error("Cannot calculate statistics for empty data.");
    }

    const ValueType absoluteSum = std::accumulate(
        data.cbegin(),
        data.cend(),
        ValueType(),
        [](const ValueType& sum, const ValueType& value)
        {
            return sum + std::abs(value);
        }
    );

    data.push_back(absoluteSum);
    data.push_back(absoluteSum / static_cast<ValueType>(elementCount));
}
}

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

template <typename Container>
Container modifyContainer(const Container& source)
{
    typedef typename Container::value_type ValueType;
    const ValueType divisor = detail::calculateDivisor(
        source.cbegin(),
        source.cend()
    );

    Container result(source);

    for (ValueType& value : result)
    {
        value /= divisor;
    }

    detail::appendAbsoluteStatistics(result);
    return result;
}

template <typename Iterator, typename Container>
void modifyWithIterators(Iterator begin, Iterator end, Container& result)
{
    typedef typename std::iterator_traits<Iterator>::value_type SourceValueType;
    typedef typename Container::value_type ResultValueType;

    const SourceValueType divisor = detail::calculateDivisor(begin, end);
    result.clear();

    for (Iterator position = begin; position != end; ++position)
    {
        result.push_back(
            static_cast<ResultValueType>(*position / divisor)
        );
    }

    detail::appendAbsoluteStatistics(result);
}

template <typename Container>
Container modifyWithForEach(const Container& source)
{
    typedef typename Container::value_type ValueType;
    const ValueType divisor = detail::calculateDivisor(
        source.cbegin(),
        source.cend()
    );

    Container result(source);

    std::for_each(
        result.begin(),
        result.end(),
        [divisor](ValueType& value)
        {
            value /= divisor;
        }
    );

    detail::appendAbsoluteStatistics(result);
    return result;
}

void modifyWithTransform(
    const std::string& inputFileName,
    const std::string& outputFileName
);
void runMenu();
}

#endif

