#ifndef INCLUDE_MATRIX_H_
#define INCLUDE_MATRIX_H_

#include <cstddef>
#include <iostream>
#include <math.h>
#include <vector.h>

template <std::size_t N>
class Matrix
{
  public:
    enum Axis3D
    {
        x,
        y,
        z
    };

    Matrix()
    {
        InitialiseScalar(0);
    };

    Matrix(const double scalar)
    {
        InitialiseScalar(scalar);
    };

    Matrix(const double n[N][N])
    {
        for (std::size_t i = 0; i < N; i++)
        {
            for (std::size_t j = 0; j < N; j++)
            {
                values[i][j] = n[i][j];
            }
        }
    };

    Matrix(const Matrix<N> &obj);

    ~Matrix(){};

    const double &operator()(int i, int j) const;

    double &operator()(int i, int j);

    static std::unique_ptr<Matrix<4>> HomoRotMatix4D(Axis3D axis, double angle);

    static std::unique_ptr<Matrix<4>> HomoTransMatrix4D(double x, double y, double z);

    Matrix<N> operator*(const Matrix<N> &B)
    {
        Matrix<N> C;

        for (std::size_t i = 0; i < N; i++)
        {
            for (std::size_t j = 0; j < N; j++)
            {
                double sum = 0;
                for (std::size_t k = 0; k < N; k++)
                {
                    sum += values[k][j] * B(i, k);
                }
                C(i, j) = sum;
            }
        }

        return C;
    };

    double *operator*(const double (&B)[N])
    {
        double *C = new double[N];

        for (std::size_t i = 0; i < N; i++)
        {
            double sum = 0;
            for (std::size_t j = 0; j < N; j++)
            {
                sum += values[j][i] * B[j];
            }
            C[i] = sum;
        }

        return C;
    };

    Vector<N> operator*(const Vector<N> &B)
    {
        Vector<N> C;

        for (std::size_t i = 0; i < N; i++)
        {
            double sum = 0;
            for (std::size_t j = 0; j < N; j++)
            {
                sum += values[j][i] * B[j];
            }
            C[i] = sum;
        }

        return C;
    };

    void operator=(const double (&array)[N][N])
    {
        for (std::size_t i = 0; i < N; ++i)
        {
            for (std::size_t j = 0; j < N; ++j)
            {
                values[i][j] = array[i][j];
            }
        }
    };

  private:
    double values[N][N];
    void InitialiseScalar(double scalar);
};

#endif