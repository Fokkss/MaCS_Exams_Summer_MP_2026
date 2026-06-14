#pragma once

#include <cstddef>
#include <iostream>
#include <stdexcept>

using namespace std;

template <typename T, size_t Rows, size_t Cols>
class StaticMatrix
{
private:
    T data_[Rows * Cols];

    size_t index(size_t row, size_t col) const
    {
        if (row >= Rows || col >= Cols)
        {
            throw out_of_range("StaticMatrix index is out of range");
        }

        return row * Cols + col;
    }

public:
    StaticMatrix()
        : data_{}
    {
    }

    size_t rows() const
    {
        return Rows;
    }

    size_t cols() const
    {
        return Cols;
    }

    T& operator()(size_t row, size_t col)
    {
        return data_[index(row, col)];
    }

    const T& operator()(size_t row, size_t col) const
    {
        return data_[index(row, col)];
    }

    void fill(const T& value)
    {
        for (size_t i = 0; i < Rows * Cols; ++i)
        {
            data_[i] = value;
        }
    }

    StaticMatrix& operator+=(const StaticMatrix& other)
    {
        for (size_t i = 0; i < Rows * Cols; ++i)
        {
            data_[i] += other.data_[i];
        }

        return *this;
    }

    void print(ostream& out) const
    {
        for (size_t row = 0; row < Rows; ++row)
        {
            for (size_t col = 0; col < Cols; ++col)
            {
                out << (*this)(row, col) << " ";
            }

            out << endl;
        }
    }
};

template <typename T, size_t Rows, size_t Cols>
StaticMatrix<T, Rows, Cols> operator+(
    StaticMatrix<T, Rows, Cols> left,
    const StaticMatrix<T, Rows, Cols>& right
)
{
    left += right;
    return left;
}

template <typename T, size_t Rows, size_t Cols>
T trace(const StaticMatrix<T, Rows, Cols>& matrix)
{
    static_assert(Rows == Cols, "trace requires square matrix");

    T result = {};

    for (size_t i = 0; i < Rows; ++i)
    {
        result += matrix(i, i);
    }

    return result;
}

template <typename T>
using Matrix3x3 = StaticMatrix<T, 3, 3>;

