#pragma once

#include "Vector.h"

class ProxyVector {
public:
    ProxyVector(Vector& vec, std::size_t step);

    double& operator()(std::size_t index);
    const double& operator()(std::size_t index) const;

    std::size_t size() const;

private:
    Vector& m_vec;
    std::size_t m_step;
};