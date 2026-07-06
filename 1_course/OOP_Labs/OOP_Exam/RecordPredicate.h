#ifndef RECORD_PREDICATE_H
#define RECORD_PREDICATE_H

#include <string>
#include <tuple>

using Record = std::tuple<int, std::string, double>;

// Функтор - объект, который можно вызвать как функцию через operator().
// Он удобен, когда предикату нужно хранить состояние, здесь - искомую строку.
class ContainsStringFunctor
{
public:
    // text будет искаться во втором поле Record, то есть в std::string.
    explicit ContainsStringFunctor(const std::string& text);

    bool operator()(const Record& record) const;

private:
    std::string m_text;
};

#endif
