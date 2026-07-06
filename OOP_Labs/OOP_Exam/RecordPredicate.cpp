#include "RecordPredicate.h"

ContainsStringFunctor::ContainsStringFunctor(const std::string& text)
    : m_text(text)
{
    // Список инициализации сразу создает поле m_text нужным значением.
    // Здесь запоминаем строку из условия предиката: что именно надо искать.
}

bool ContainsStringFunctor::operator()(const Record& record) const
{
    // operator() делает объект функтором: его можно вызывать как функцию.
    // const означает, что проверка записи не меняет состояние предиката.
    // std::string::npos означает "подстрока не найдена".
    // std::get<1> - это второе поле tuple, именно там по заданию ищем строку.
    return std::get<1>(record).find(m_text) != std::string::npos;
}
