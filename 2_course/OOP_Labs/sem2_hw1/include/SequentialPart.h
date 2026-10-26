// Защита от повторного подключения заголовочного файла.
#ifndef SEQUENTIAL_PART_H
#define SEQUENTIAL_PART_H

// Стандартные средства для алгоритмов, итераторов, потоков и ошибок.
#include <algorithm>
#include <cmath>
#include <fstream>
#include <iomanip>
#include <iterator>
#include <numeric>
#include <stdexcept>
#include <string>

// Пространство имён первой части задания.
namespace sequential
{
// Префикс std:: показывает, что string и другие имена взяты из стандартной
// библиотеки C++, а не объявлены в нашей программе.
// Общие имена входного и выходного файлов.
const std::string INPUT_FILE_NAME = "input.txt";
const std::string OUTPUT_FILE_NAME = "output.txt";

// detail хранит внутренние шаблонные функции, не предназначенные для меню.
namespace detail
{
// template объявляет шаблон: вместо Iterator можно подставить тип итератора
// vector, deque, list или другого подходящего контейнера.
template <typename Iterator>
// typename сообщает компилятору, что value_type внутри iterator_traits — тип.
typename std::iterator_traits<Iterator>::value_type calculateDivisor(
    Iterator begin,
    Iterator end
)
{
    // typedef создаёт короткое имя типа элемента, на который указывает итератор.
    typedef typename std::iterator_traits<Iterator>::value_type ValueType;

    // distance считает число элементов между двумя итераторами.
    if (std::distance(begin, end) < 50)
    {
        throw std::runtime_error("At least 50 values are required.");
    }

    // find_if возвращает итератор на первый элемент, подходящий под условие.
    Iterator firstNegative = std::find_if(
        begin,
        end,
        // [] обозначает лямбда-функцию. Пустые скобки означают, что она
        // не захватывает внешние переменные.
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
    // Смещение 49 от begin соответствует элементу с порядковым номером 50.
    std::advance(fiftieth, 49);

    // static_cast создаёт число 2 нужного типа и сохраняет дробную часть.
    const ValueType divisor =
        (*firstNegative + *fiftieth) / static_cast<ValueType>(2);

    if (divisor == ValueType())
    {
        throw std::runtime_error("The division value is zero.");
    }

    return divisor;
}

// Шаблон принимает любой последовательный контейнер с push_back.
template <typename Container>
void appendAbsoluteStatistics(Container& data)
{
    // Container::value_type — тип одного элемента контейнера.
    typedef typename Container::value_type ValueType;
    // Container::size_type — тип, которым контейнер хранит свой размер.
    const typename Container::size_type elementCount = data.size();

    if (elementCount == 0)
    {
        throw std::runtime_error("Cannot calculate statistics for empty data.");
    }

    // accumulate проходит диапазон и накапливает сумму модулей.
    const ValueType absoluteSum = std::accumulate(
        data.cbegin(),
        data.cend(),
        ValueType(),
        [](const ValueType& sum, const ValueType& value)
        {
            return sum + std::abs(value);
        }
    );

    // push_back добавляет два итоговых значения в конец результата.
    data.push_back(absoluteSum);
    data.push_back(absoluteSum / static_cast<ValueType>(elementCount));
}
}

// Две обычные функции создания исходного файла.
void createDataWithStreams(const std::string& fileName);
void createDataWithAlgorithms(const std::string& fileName);

// Загружает данные обычным чтением потока в любой подходящий Container.
template <typename Container>
Container loadDataWithStreams(const std::string& fileName)
{
    std::ifstream input(fileName);

    if (!input)
    {
        throw std::runtime_error("Cannot open the input data file.");
    }

    Container data;
    // typename обязателен: value_type зависит от параметра шаблона Container.
    typename Container::value_type value{};

    // Цикл продолжается, пока очередное значение успешно прочитано.
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

// Загружает данные через потоковый итератор и стандартный алгоритм copy.
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

    // istream_iterator читает значения из файла как последовательность.
    // back_inserter вызывает push_back у выбранного контейнера.
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

// Первая версия modify: принимает контейнер и возвращает такой же тип.
template <typename Container>
Container modifyContainer(const Container& source)
{
    typedef typename Container::value_type ValueType;
    const ValueType divisor = detail::calculateDivisor(
        source.cbegin(),
        source.cend()
    );

    // Создаём копию, чтобы исходные данные не изменились.
    Container result(source);

    // Range-based for последовательно получает каждый элемент контейнера.
    // Амперсанд означает ссылку, поэтому меняется сам элемент, а не его копия.
    for (ValueType& value : result)
    {
        value /= divisor;
    }

    detail::appendAbsoluteStatistics(result);
    return result;
}

// Вторая версия modify работает только с границами диапазона итераторов.
template <typename Iterator, typename Container>
// Знак & у result означает передачу по ссылке: функция заполняет исходный
// контейнер результата, а не его временную копию.
void modifyWithIterators(Iterator begin, Iterator end, Container& result)
{
    // iterator_traits получает тип элемента независимо от вида итератора.
    typedef typename std::iterator_traits<Iterator>::value_type SourceValueType;
    typedef typename Container::value_type ResultValueType;

    const SourceValueType divisor = detail::calculateDivisor(begin, end);
    result.clear(); // Удаляем старое содержимое результирующего контейнера.

    // ++position переводит итератор к следующему элементу.
    for (Iterator position = begin; position != end; ++position)
    {
        result.push_back(
            // * разыменовывает итератор, то есть получает текущее значение.
            static_cast<ResultValueType>(*position / divisor)
        );
    }

    detail::appendAbsoluteStatistics(result);
}

// Третья версия modify использует алгоритм for_each.
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
        // [divisor] копирует делитель внутрь лямбда-функции.
        [divisor](ValueType& value)
        {
            value /= divisor;
        }
    );

    detail::appendAbsoluteStatistics(result);
    return result;
}

// Записывает результат обычным циклом и файловым потоком.
template <typename Container>
void outputResultWithStreams(
    const Container& data,
    const std::string& fileName
)
{
    std::ofstream output(fileName);

    if (!output)
    {
        throw std::runtime_error("Cannot open the result file for writing.");
    }

    // setprecision задаёт одинаковую точность для вещественных чисел.
    output << std::setprecision(15);

    // const ссылка позволяет читать элемент без копирования и изменения.
    for (const typename Container::value_type& value : data)
    {
        output << value << '\n';
    }

    if (!output)
    {
        throw std::runtime_error("Cannot write all result data to the file.");
    }
}

// Записывает результат алгоритмом copy через ostream_iterator.
template <typename Container>
void outputResultWithIterators(
    const Container& data,
    const std::string& fileName
)
{
    std::ofstream output(fileName);

    if (!output)
    {
        throw std::runtime_error("Cannot open the result file for writing.");
    }

    output << std::setprecision(15);

    // cbegin и cend возвращают итераторы только для чтения.
    std::copy(
        data.cbegin(),
        data.cend(),
        std::ostream_iterator<typename Container::value_type>(output, "\n")
    );

    if (!output)
    {
        throw std::runtime_error("Cannot write all result data to the file.");
    }
}

// Четвёртая версия modify читает и записывает файл потоковыми итераторами.
void modifyWithTransform(
    const std::string& inputFileName,
    const std::string& outputFileName
);
// Запускает подменю первой части.
void runMenu();
}

#endif // SEQUENTIAL_PART_H

