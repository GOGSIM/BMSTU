#pragma once

#include <cstddef>
#include <iosfwd>

class CSLRMatrix;

class Vector {
public:
    Vector();             
    explicit Vector(std::size_t size, double initialValue = 0.0);// что явл. целевым и исходными типами +
    Vector(const Vector& other);//                                  
    ~Vector();

    Vector& operator=(const Vector& other);

    std::size_t size() const;
    double length() const;

    double& operator[](std::size_t index);
    const double& operator[](std::size_t index) const; // const в конце методов, зачем, что делает +
//                                                    
    const Vector& operator+() const;
    Vector operator-() const;

    Vector& operator+=(const Vector& other);
    Vector& operator-=(const Vector& other);

    explicit operator double() const; //за что отвечает этот оператор +

    //куда перелается указатеьл this , передается и он в друж фун +

    friend Vector operator+(const Vector& lhs, const Vector& rhs);
    friend Vector operator-(const Vector& lhs, const Vector& rhs);

    friend Vector operator*(const Vector& vector, double scalar);
    friend Vector operator*(double scalar, const Vector& vector);
    friend double operator*(const Vector& lhs, const Vector& rhs);
    friend Vector operator*(const Vector& vector, const CSLRMatrix& matrix);

    friend std::ostream& operator<<(std::ostream& out, const Vector& vector);
    friend std::istream& operator>>(std::istream& in, Vector& vector);

private:
    void checkIndex(std::size_t index) const;
    void checkSameSize(const Vector& other, const char* operation) const;

private:
    std::size_t m_size;
    double* m_data; // какие методы обязаны предоставить еси есть указатель среди полей +
};                 

// спец методы, их виды, что сделает компилятор, если мы их не предостсвтвиоли 

// => спец конструкторы:
// создание, копирование, перемещение, уничтожение, присваивание
// конструктор, декструктор, копириования, оп. присваивания, констр. перемещения
// компилятор автомат. генерирует конструктор, десткрутор, копирования, оп. присваивания
// НО: при динамике: копирования - копирует тоьлко указатель -> двойное удаление 