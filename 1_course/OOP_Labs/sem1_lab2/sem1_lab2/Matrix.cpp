#include "Matrix.h"

#include "Exceptions.h"
#include "Vector.h"

#include <cmath>
#include <iostream>
#include <sstream>

CSLRMatrix::CSLRMatrix()
    : m_size(0),
      m_profileSize(0),
      m_di(nullptr),
      m_al(nullptr),
      m_au(nullptr),
      m_iptr(nullptr),
      m_jptr(nullptr) {
}

CSLRMatrix::CSLRMatrix(std::size_t size,
                       const double* di,
                       const double* al,
                       const double* au,
                       const std::size_t* iptr,
                       const std::size_t* jptr)
    : m_size(size),
      m_profileSize(iptr[size]),
      m_di(nullptr),
      m_al(nullptr),
      m_au(nullptr),
      m_iptr(nullptr),
      m_jptr(nullptr) {
    allocate(m_size, m_profileSize);

    for (std::size_t i = 0; i < m_size; ++i) {
        m_di[i] = di[i];
        m_iptr[i] = iptr[i];
    }
    m_iptr[m_size] = iptr[m_size];

    for (std::size_t i = 0; i < m_profileSize; ++i) {
        m_al[i] = al[i];
        m_au[i] = au[i];
        m_jptr[i] = jptr[i];
    }

    validateStructure();
}

CSLRMatrix& CSLRMatrix::operator=(const CSLRMatrix& other) {
    if (this == &other) {
        return *this;
    }

    if (m_size != other.m_size || m_profileSize != other.m_profileSize) {
        free();
        allocate(other.m_size, other.m_profileSize);
    }

    for (std::size_t i = 0; i < m_size; ++i) {
        m_di[i] = other.m_di[i];
        m_iptr[i] = other.m_iptr[i];
    }
    m_iptr[m_size] = other.m_iptr[m_size];

    for (std::size_t i = 0; i < m_profileSize; ++i) {
        m_al[i] = other.m_al[i];
        m_au[i] = other.m_au[i];
        m_jptr[i] = other.m_jptr[i];
    }

    return *this;
}

std::size_t CSLRMatrix::size() const {
    return m_size;
}

std::size_t CSLRMatrix::nonZeroCount() const {
    return m_size == 0 ? 0 : m_iptr[m_size];
}

double CSLRMatrix::at(std::size_t row, std::size_t col) const {
    if (row >= m_size || col >= m_size) {
        std::ostringstream oss;
        oss << "Matrix index (" << row << ", " << col << ") is out of range for size " << m_size << '.';
        throw OutOfRangeException(oss.str());
    }

    if (row == col) {
        return m_di[row];
    }

    if (row > col) {
        for (std::size_t k = m_iptr[row]; k < m_iptr[row + 1]; ++k) {
            if (m_jptr[k] == col) {
                return m_al[k];
            }
        }
        return 0.0;
    }

    for (std::size_t k = m_iptr[col]; k < m_iptr[col + 1]; ++k) {
        if (m_jptr[k] == row) {
            return m_au[k];
        }
    }
    return 0.0;
}

CSLRMatrix operator*(const CSLRMatrix& matrix, double scalar) {
    CSLRMatrix result(matrix);
    for (std::size_t i = 0; i < result.m_size; ++i) {
        result.m_di[i] *= scalar;
    }
    for (std::size_t i = 0; i < result.m_profileSize; ++i) {
        result.m_al[i] *= scalar;
        result.m_au[i] *= scalar;
    }
    return result;
}

CSLRMatrix operator*(double scalar, const CSLRMatrix& matrix) {
    return matrix * scalar;
}

Vector operator*(const CSLRMatrix& matrix, const Vector& vector) {
    if (matrix.size() != vector.size()) {
        std::ostringstream oss;
        oss << "Matrix-vector multiplication requires compatible dimensions: matrix size = "
            << matrix.size() << ", vector size = " << vector.size() << '.';
        throw IncompatibleDimException(oss.str());
    }

    Vector result(matrix.size(), 0.0);
    std::size_t j = 0;

    for (std::size_t i = 0; i < matrix.m_size; ++i) {
        result[i] += matrix.m_di[i] * vector[i];
        for (std::size_t k = matrix.m_iptr[i]; k < matrix.m_iptr[i + 1]; ++k) {
            j = matrix.m_jptr[k];
            result[i] += matrix.m_al[k] * vector[j];
            result[j] += matrix.m_au[k] * vector[i];
        }
    }

    return result;
}

std::ostream& operator<<(std::ostream& out, const CSLRMatrix& matrix) {
    for (std::size_t i = 0; i < matrix.m_size; ++i) {
        std::size_t k = matrix.m_iptr[i];
        std::size_t kEnd = matrix.m_iptr[i + 1];

        for (std::size_t j = 0; j < matrix.m_size; ++j) {
            double value = 0.0;

            if (i == j) {
                value = matrix.m_di[i];
            } else if (i > j) {
                if (k < kEnd && matrix.m_jptr[k] == j) {
                    value = matrix.m_al[k];
                    ++k;
                }
            } else {
                value = matrix.at(i, j);
            }

            out << value;
            if (j + 1 < matrix.m_size) {
                out << '\t';
            }
        }
        out << std::endl;
    }

    return out;
}

std::istream& operator>>(std::istream& in, CSLRMatrix& matrix) {
    std::size_t size = 0;
    in >> size;
    if (!in) {
        return in;
    }

    CSLRMatrix temp;
    temp.m_size = size;
    temp.m_di = new double[temp.m_size];
    temp.m_iptr = new std::size_t[temp.m_size + 1];

    for (std::size_t i = 0; i < temp.m_size; ++i) {
        in >> temp.m_di[i];
    }
    for (std::size_t i = 0; i <= temp.m_size; ++i) {
        in >> temp.m_iptr[i];
    }

    if (!in) {
        return in;
    }

    temp.m_profileSize = temp.m_iptr[temp.m_size];
    temp.m_al = new double[temp.m_profileSize];
    temp.m_au = new double[temp.m_profileSize];
    temp.m_jptr = new std::size_t[temp.m_profileSize];

    for (std::size_t i = 0; i < temp.m_profileSize; ++i) {
        in >> temp.m_al[i];
    }
    for (std::size_t i = 0; i < temp.m_profileSize; ++i) {
        in >> temp.m_au[i];
    }
    for (std::size_t i = 0; i < temp.m_profileSize; ++i) {
        in >> temp.m_jptr[i];
    }

    if (!in) {
        return in;
    }

    temp.validateStructure();
    matrix = temp;
    return in;
}

void CSLRMatrix::allocate(std::size_t size, std::size_t profileSize) {
    m_size = size;
    m_profileSize = profileSize;
    m_di = new double[m_size];
    m_al = new double[m_profileSize];
    m_au = new double[m_profileSize];
    m_iptr = new std::size_t[m_size + 1];
    m_jptr = new std::size_t[m_profileSize];
}

void CSLRMatrix::free() {
    delete[] m_di;
    delete[] m_al;
    delete[] m_au;
    delete[] m_iptr;
    delete[] m_jptr;

    m_size = 0;
    m_profileSize = 0;
    m_di = nullptr;
    m_al = nullptr;
    m_au = nullptr;
    m_iptr = nullptr;
    m_jptr = nullptr;
}

void CSLRMatrix::validateStructure() const {
    if (m_size == 0) {
        return;
    }

    if (m_iptr[0] != 0) {
        throw IncompatibleDimException("CSLR structure is invalid: iptr[0] must be 0.");
    }

    if (m_iptr[m_size] != m_profileSize) {
        throw IncompatibleDimException("CSLR structure is invalid: iptr[size] must be equal to profile size.");
    }

    for (std::size_t i = 0; i < m_size; ++i) {
        if (m_iptr[i] > m_iptr[i + 1]) {
            throw IncompatibleDimException("CSLR structure is invalid: iptr must be nondecreasing.");
        }

        for (std::size_t k = m_iptr[i]; k < m_iptr[i + 1]; ++k) {
            if (m_jptr[k] >= i) {
                throw IncompatibleDimException("CSLR structure is invalid: jptr must contain only indices below diagonal.");
            }
            if (k > m_iptr[i] && m_jptr[k - 1] >= m_jptr[k]) {
                throw IncompatibleDimException("CSLR structure is invalid: jptr values inside each row must be strictly increasing.");
            }
        }
    }
}