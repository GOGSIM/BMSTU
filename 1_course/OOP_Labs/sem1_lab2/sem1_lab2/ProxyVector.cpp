#include "ProxyVector.h"

#include "Exceptions.h"
#include <string>

ProxyVector::ProxyVector(Vector& vec, std::size_t step)
    : m_vec(vec), m_step(step) {
    if (step == 0) {
        throw IncompatibleDimException("Step cannot be zero.");
    }
}

std::size_t ProxyVector::size() const {
    return (m_vec.size() + m_step - 1) / m_step;
}

double& ProxyVector::operator()(std::size_t index) {
    const std::size_t realIndex = index * m_step;

    if (realIndex >= m_vec.size()) {
        throw OutOfRangeException(
            std::string("Proxy index out of range: ")
            + std::to_string(index)
        );
    }

    return m_vec[realIndex];
}

const double& ProxyVector::operator()(std::size_t index) const {
    const std::size_t realIndex = index * m_step;

    if (realIndex >= m_vec.size()) {
        throw OutOfRangeException(
            std::string("Proxy index out of range: ")
            + std::to_string(index)
        );
    }

    return m_vec[realIndex];
}