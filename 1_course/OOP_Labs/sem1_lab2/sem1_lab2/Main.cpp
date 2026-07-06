#include <iostream>
#include <string>

#include "Vector.h"
#include "Matrix.h"
#include "Exceptions.h"
#include "ProxyVector.h"

void printHeader(const std::string& title) {
    std::cout << std::endl;
    std::cout << "========== " << title << " ==========" << std::endl;
}

int main() {
    try {
        // cd BMSTU/Basic_C++/sem1_lab2/sem1_lab2
        // g++ Main.cpp Vector.cpp Matrix.cpp Exceptions.cpp -std=c++17 -o lab2.exe
        // .\lab2.exe
        printHeader("Vector constructors and basic methods");

        Vector x(3, 0.0);
        x[0] = 1.0;
        x[1] = 2.0;
        x[2] = 3.0;

        Vector y(3, 1.5);
        Vector z(x);
        Vector assigned;
        assigned = y;

        std::cout << "x = " << x << std::endl;
        std::cout << "y = " << y << std::endl;
        std::cout << "z (copy of x) = " << z << std::endl;
        std::cout << "assigned (after = y) = " << assigned << std::endl;

        printHeader("Vector operations");

        std::cout << "+x = " << (+x) << std::endl;
        std::cout << "-x = " << (-x) << std::endl;
        std::cout << "x + y = " << (x + y) << std::endl;
        std::cout << "x - y = " << (x - y) << std::endl;
        std::cout << "Dot product (x * y) = " << (x * y) << std::endl;
        std::cout << "Length of x = " << x.length() << std::endl;
        std::cout << "double(x) = " << static_cast<double>(x) << std::endl;

        printHeader("Scalar operations");

        std::cout << "x * 2 = " << (x * 2.0) << std::endl;
        std::cout << "2 * x = " << (2.0 * x) << std::endl;

        printHeader("Matrix construction");

        const std::size_t size = 3;

        double di[] = {10.0, 20.0, 30.0};
        double al[] = {1.0, 2.0, 3.0};
        double au[] = {4.0, 5.0, 6.0};
        std::size_t iptr[] = {0, 0, 1, 3};
        std::size_t jptr[] = {0, 0, 1};

        CSLRMatrix matrix(size, di, al, au, iptr, jptr);

        std::cout << "Matrix:" << std::endl;
        std::cout << matrix << std::endl;

        printHeader("Matrix-vector multiplication");

        Vector mv = matrix * x;
        std::cout << "matrix * x = " << mv << std::endl;

        printHeader("Vector-matrix multiplication");

        Vector vm = x * matrix;
        std::cout << "x * matrix = " << vm << std::endl;

        printHeader("Matrix scalar multiplication");

        CSLRMatrix scaled1 = matrix * 2.0;
        CSLRMatrix scaled2 = 3.0 * matrix;

        std::cout << "matrix * 2:" << std::endl;
        std::cout << scaled1 << std::endl;

        std::cout << "3 * matrix:" << std::endl;
        std::cout << scaled2 << std::endl;

        printHeader("Complex expression");

        Vector expr = (matrix * x) + (2.0 * y) - (-x);
        std::cout << "(matrix * x) + (2 * y) - (-x) = " << expr << std::endl;

        printHeader("ProxyVector test");

        Vector v(10, 0.0);
        for (std::size_t i = 0; i < v.size(); ++i) {
            v[i] = static_cast<double>(i + 1);
        }

        std::cout << "Initial v = " << v << std::endl;

        ProxyVector proxy(v, 3);
        proxy(0) *= 10.0;
        proxy(1) *= 10.0;
        proxy(2) *= 10.0;

        std::cout << "v after proxy modifications = " << v << std::endl;
        std::cout << "proxy size = " << proxy.size() << std::endl;
        std::cout << "proxy(0) = " << proxy(0) << std::endl;
        std::cout << "proxy(1) = " << proxy(1) << std::endl;
        std::cout << "proxy(2) = " << proxy(2) << std::endl;

        printHeader("Exception tests");

        try {
            std::cout << "Attempt to access x[10]" << std::endl;
            std::cout << x[10] << std::endl;
        } catch (const OutOfRangeException& e) {
            std::cout << "Caught OutOfRangeException: " << e.what() << std::endl;
        }

        try {
            Vector shortVec(2, 1.0);
            std::cout << "Attempt to add x + shortVec" << std::endl;
            std::cout << (x + shortVec) << std::endl;
        } catch (const IncompatibleDimException& e) {
            std::cout << "Caught IncompatibleDimException: " << e.what() << std::endl;
        }

        try {
            Vector badVec(4, 1.0);
            std::cout << "Attempt to multiply matrix * badVec" << std::endl;
            std::cout << (matrix * badVec) << std::endl;
        } catch (const IncompatibleDimException& e) {
            std::cout << "Caught IncompatibleDimException: " << e.what() << std::endl;
        }

        try {
            std::cout << "Attempt to create ProxyVector with step = 0" << std::endl;
            ProxyVector badProxy(v, 0);
            std::cout << badProxy.size() << std::endl;
        } catch (const std::exception& e) {
            std::cout << "Caught exception: " << e.what() << std::endl;
        }

    } catch (const std::exception& e) {
        std::cout << "Unhandled exception: " << e.what() << std::endl;
    }

    return 0;
}