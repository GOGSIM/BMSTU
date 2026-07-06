#ifndef REPLACE_IF_H
#define REPLACE_IF_H

#include <iterator>

template<typename Iter, typename UnaryPredicate, typename NewValue>
// Iter - тип итератора, UnaryPredicate - тип условия, NewValue - тип нового значения.
void replaceIf(Iter iterBeg, Iter iterEnd, UnaryPredicate predicate, const NewValue& newValue)
{
    // predicate - условие замены, newValue - значение для замены.
    // Диапазон задается как в STL: [iterBeg, iterEnd).
    for (; iterBeg != iterEnd; ++iterBeg)
    {
        // Здесь вызывается UnaryPredicate для текущего элемента.
        if (predicate(*iterBeg))
        {
            // Элемент меняем через итератор. newValue передан по const&,
            // чтобы не копировать большую запись без необходимости.
            // Сложность O(n): каждый элемент диапазона проверяется один раз.
            *iterBeg = newValue;
        }
    }
}

#endif
