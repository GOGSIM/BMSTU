#include "Vector.h"

#include "Exceptions.h"
#include "Matrix.h"

#include <cmath>
#include <iostream>
#include <string>

Vector::Vector()
    : m_size(0),
      m_data(nullptr) {
}

Vector::Vector(std::size_t size, double initialValue)
    : m_size(size),
      m_data(new double[m_size]) {
    for (std::size_t i = 0; i < m_size; ++i) {
        m_data[i] = initialValue;
    }
}

Vector::Vector(const Vector& other)
    : m_size(other.m_size),
      m_data(new double[m_size]) {
    for (std::size_t i = 0; i < m_size; ++i) {
        m_data[i] = other.m_data[i];
    }
}

Vector::~Vector() {
    delete[] m_data;
}

Vector& Vector::operator=(const Vector& other) {
    if (this == &other) {
        return *this;
    }

    if (m_size != other.m_size) {
        delete[] m_data;
        m_size = other.m_size;
        m_data = new double[m_size];
    }

    for (std::size_t i = 0; i < m_size; ++i) {
        m_data[i] = other.m_data[i];
    }

    return *this;
}

std::size_t Vector::size() const {
    return m_size;
}

double Vector::length() const {
    double sum = 0.0;
    for (std::size_t i = 0; i < m_size; ++i) {
        sum += m_data[i] * m_data[i];
    }
    return std::sqrt(sum);
}

double& Vector::operator[](std::size_t index) {
    checkIndex(index);
    return m_data[index];
}

const double& Vector::operator[](std::size_t index) const {
    checkIndex(index);
    return m_data[index];
}

const Vector& Vector::operator+() const {
    return *this;
}

Vector Vector::operator-() const {
    Vector result(*this);
    for (std::size_t i = 0; i < result.m_size; ++i) {
        result.m_data[i] *= -1.0;
    }
    return result;
}

Vector& Vector::operator+=(const Vector& other) {
    checkSameSize(other, "Vector addition requires vectors of the same size.");
    for (std::size_t i = 0; i < m_size; ++i) {
        m_data[i] += other.m_data[i];
    }
    return *this;
}

Vector& Vector::operator-=(const Vector& other) {
    checkSameSize(other, "Vector subtraction requires vectors of the same size.");
    for (std::size_t i = 0; i < m_size; ++i) {
        m_data[i] -= other.m_data[i];
    }
    return *this;
}

Vector::operator double() const {
    return length();
}

Vector operator+(const Vector& lhs, const Vector& rhs) {
    Vector result(lhs);
    result += rhs;
    return result;
}

Vector operator-(const Vector& lhs, const Vector& rhs) {
    Vector result(lhs);
    result -= rhs;
    return result;
}

Vector operator*(const Vector& vector, double scalar) {
    Vector result(vector);
    for (std::size_t i = 0; i < result.m_size; ++i) {
        result.m_data[i] *= scalar;
    }
    return result;
}

Vector operator*(double scalar, const Vector& vector) {
    return vector * scalar;
}

double operator*(const Vector& lhs, const Vector& rhs) {
    lhs.checkSameSize(rhs, "Dot product requires vectors of the same size.");

    double result = 0.0;
    for (std::size_t i = 0; i < lhs.m_size; ++i) {
        result += lhs.m_data[i] * rhs.m_data[i];
    }
    return result;
}

Vector operator*(const Vector& vector, const CSLRMatrix& matrix) {
    if (vector.size() != matrix.size()) {
        throw IncompatibleDimException(
            std::string("Vector-matrix multiplication requires compatible dimensions: vector size = ")
            + std::to_string(vector.size())
            + ", matrix size = "
            + std::to_string(matrix.size())
            + "."
        );
    }

    Vector result(matrix.m_size, 0.0);

    for (std::size_t i = 0; i < matrix.m_size; ++i) {
        result[i] += vector[i] * matrix.m_di[i];

        for (std::size_t k = matrix.m_iptr[i]; k < matrix.m_iptr[i + 1]; ++k) {
            const std::size_t j = matrix.m_jptr[k];
            result[j] += vector[i] * matrix.m_al[k];
            result[i] += vector[j] * matrix.m_au[k];
        }
    }

    return result;
}

std::ostream& operator<<(std::ostream& out, const Vector& vector) {
    out << '[';
    for (std::size_t i = 0; i < vector.m_size; ++i) {
        if (i > 0) {
            out << ", ";
        }
        out << vector.m_data[i];
    }
    out << ']';
    return out;
}

std::istream& operator>>(std::istream& in, Vector& vector) {
    std::size_t size = 0;
    in >> size;
    if (!in) {
        return in;
    }

    if (vector.m_size != size) {
        delete[] vector.m_data;
        vector.m_size = size;
        vector.m_data = new double[vector.m_size];
    }

    for (std::size_t i = 0; i < vector.m_size; ++i) {
        in >> vector.m_data[i];
    }

    return in;
}

void Vector::checkIndex(std::size_t index) const {
    if (index >= m_size) {
        throw OutOfRangeException(
            std::string("Vector index ")
            + std::to_string(index)
            + " is out of range for size "
            + std::to_string(m_size)
            + "."
        );
    }
}

void Vector::checkSameSize(const Vector& other, const char* operation) const {
    if (m_size != other.m_size) {
        throw IncompatibleDimException(
            std::string(operation)
            + " Left size = "
            + std::to_string(m_size)
            + ", right size = "
            + std::to_string(other.m_size)
            + "."
        );
    }
}