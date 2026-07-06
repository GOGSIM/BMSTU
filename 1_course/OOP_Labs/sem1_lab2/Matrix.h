#pragma once

#include <cstddef>
#include <iosfwd>

class Vector;

class CSLRMatrix {
public:
    CSLRMatrix();
    CSLRMatrix(std::size_t size,
               const double* di,
               const double* al,
               const double* au,
               const std::size_t* iptr,
               const std::size_t* jptr);
    CSLRMatrix(const CSLRMatrix& other);
    ~CSLRMatrix();

    CSLRMatrix& operator=(const CSLRMatrix& other);

    std::size_t size() const;
    std::size_t nonZeroCount() const;

    double at(std::size_t row, std::size_t col) const;

    friend CSLRMatrix operator*(const CSLRMatrix& matrix, double scalar);
    friend CSLRMatrix operator*(double scalar, const CSLRMatrix& matrix);
    friend Vector operator*(const CSLRMatrix& matrix, const Vector& vector);
    friend Vector operator*(const Vector& vector, const CSLRMatrix& matrix);

    friend std::ostream& operator<<(std::ostream& out, const CSLRMatrix& matrix);
    friend std::istream& operator>>(std::istream& in, CSLRMatrix& matrix);

private:
    void allocate(std::size_t size, std::size_t profileSize);
    void free();
    void validateStructure() const;

private:
    static constexpr double EPS = 1e-12;

    std::size_t m_size;
    std::size_t m_profileSize;
    double* m_di;
    double* m_al;
    double* m_au;
    std::size_t* m_iptr;
    std::size_t* m_jptr;
};