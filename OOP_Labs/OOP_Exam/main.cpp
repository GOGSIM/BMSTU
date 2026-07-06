#include <iostream>
#include <iterator>
#include <string>
#include <tuple>

#include "RecordPredicate.h"
#include "ReplaceIf.h"

/*
cd OOP_Exam
g++ -std=c++2b -Wall -Wextra -pedantic main.cpp RecordPredicate.cpp -o replace_if_exam.exe
.\replace_if_exam.exe
*/

static void printRecord(const Record& record)
{
    // std::get<номер> берет элемент с известным на этапе компиляции индексом.
    std::cout << '('
              << std::get<0>(record) << ", "
              << std::get<1>(record) << ", "
              << std::get<2>(record) << ')';
}

template<std::size_t N>
static void printRecords(const Record (&records)[N])
{
    // N - нетиповой параметр шаблона: размер массива известен на этапе компиляции.
    // Передача по ссылке сохраняет размер массива, он не превращается в указатель.
    for (const Record& record : records)
    {
        printRecord(record);
        std::cout << '\n';
    }
}

int main()
{
    Record records[5] = {
        Record{1, "red apple", 10.5},
        Record{2, "green pear", 20.0},
        Record{3, "red cherry", 30.25},
        Record{4, "yellow melon", 40.75},
        Record{5, "dark red plum", 50.0}
    };

    std::cout << "Initial records:\n";
    printRecords(records);

    const Record lambdaReplacement{0, "lambda replacement", 0.0};
    // Здесь задаем NewValue для первого вызова replaceIf

    const int a = 1;

    replaceIf(
        std::begin(records),
        std::end(records),
        // Здесь задаем UnaryPredicate как лямбда-функцию.
        [a](const Record& record) mutable
        {
            // В терминах concepts это UnaryPredicate: один аргумент -> bool.
            return std::get<1>(record).find("red") != std::string::npos;
            a++;
        },
        lambdaReplacement
    );

    std::cout << "\nAfter lambda predicate:\n";
    printRecords(records);

    const Record functorReplacement{-1, "functor replacement", -1.0};
    ContainsStringFunctor containsReplacement("replacement");
    // Здесь задаем UnaryPredicate-функтор: он ищет строку "replacement".

    replaceIf(std::begin(records), std::end(records), containsReplacement, functorReplacement);

    std::cout << "\nAfter functor predicate:\n";
    printRecords(records);

    return 0;
}


template <typename _T>
    requires (sizeof(_T) > 8)
void f(){}

template <typename _T>
    requires requires { sizeof(_T) > 8; }
void f(){}

template <typename _T>
    requires requires { requires sizeof(_T) > 8; }
void f(){}