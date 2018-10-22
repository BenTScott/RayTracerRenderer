#define _USE_MATH_DEFINES
#include <math.h>
#include <memory>
#include "matrix.h"

template <std::size_t N>
void lin_alg::Matrix<N>::InitialiseScalar(double scalar)
{
    for (std::size_t j = 0; j < N; ++j)
    {
        for (std::size_t k = 0; k < N; ++k)
        {
            if (j == k)
            {
                values[j][k] = scalar;
            }
            else
            {
                values[j][k] = 0;
            }
        }
    }
};

template <std::size_t N>
lin_alg::Matrix<N>::Matrix(const Matrix<N> &obj)
{
    for (std::size_t i = 0; i < N; ++i)
    {
        for (std::size_t j = 0; j < N; ++j)
        {
            values[i][j] = obj(i, j);
        }
    }
};

template <std::size_t N>
std::unique_ptr<lin_alg::Matrix<4>> lin_alg::Matrix<N>::HomoRotMatix4D(Axis3D axis, double angle)
{
    double cos_angle = cos(angle * M_PI / 180);
    double sin_angle = sin(angle * M_PI / 180);

    std::unique_ptr<Matrix<4>> rotation_mat(new Matrix<4>());

    (*rotation_mat)(3, 3) = 1;

    if (axis == Axis3D::x)
    {
        (*rotation_mat)(0, 0) = 1;
        (*rotation_mat)(1, 1) = cos_angle;
        (*rotation_mat)(1, 2) = sin_angle;
        (*rotation_mat)(2, 1) = -sin_angle;
        (*rotation_mat)(2, 2) = cos_angle;
    }
    else if (axis == Axis3D::y)
    {
        (*rotation_mat)(0, 0) = cos_angle;
        (*rotation_mat)(0, 2) = -sin_angle;
        (*rotation_mat)(1, 1) = 1;
        (*rotation_mat)(2, 0) = sin_angle;
        (*rotation_mat)(2, 2) = cos_angle;
    }
    else
    {
        (*rotation_mat)(0, 0) = cos_angle;
        (*rotation_mat)(0, 1) = sin_angle;
        (*rotation_mat)(1, 0) = -sin_angle;
        (*rotation_mat)(1, 1) = cos_angle;
        (*rotation_mat)(2, 2) = 1;
    }

    return rotation_mat;
};

template <std::size_t N>
std::unique_ptr<lin_alg::Matrix<4>> lin_alg::Matrix<N>::HomoTransMatrix4D(double x, double y, double z)
{
    //Initialise indentity matrix
    std::unique_ptr<Matrix<4>> translation_mat(new Matrix<4>(1));

    (*translation_mat)(3, 0) = x;
    (*translation_mat)(3, 1) = y;
    (*translation_mat)(3, 2) = z;

    return translation_mat;
};

template <std::size_t N>
const double &lin_alg::Matrix<N>::operator()(int j, int k) const
{
    return values[j][k];
};

template <std::size_t N>
double &lin_alg::Matrix<N>::operator()(int j, int k)
{
    return values[j][k];
};

template class lin_alg::Matrix<4>;