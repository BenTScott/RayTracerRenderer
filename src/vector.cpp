#include <math.h>
#include "vector.h"

template <std::size_t N>
double lin_alg::Vector<N>::Magnitude() const
{
    double sum = 0;
    for (std::size_t i = 0; i < N; ++i)
    {
        double value = values[i];
        sum += pow(value, 2.0);
    }

    return sqrt(sum);
};

template <std::size_t N>
void lin_alg::Vector<N>::Normalise()
{
    double magnitude = this->Magnitude();

    for (std::size_t i = 0; i < N; ++i)
    {
        values[i] = values[i] / magnitude;
    }
};

template <std::size_t N>
double lin_alg::Vector<N>::DotProduct(const lin_alg::Vector<N> &vector) const
{
    double sum = 0;

    for (std::size_t i = 0; i < N; ++i)
    {
        sum += values[i] * vector[i];
    }

    return sum;
};

template <std::size_t N>
lin_alg::Vector<4> lin_alg::Vector<N>::CrossProduct(const Vector<4> &rhs, const Vector<4> &lhs)
{
    double x = rhs[1] * lhs[2] - rhs[2] * lhs[1];
    double y = rhs[2] * lhs[0] - rhs[0] * lhs[2];
    double z = rhs[0] * lhs[1] - rhs[1] * lhs[0];
    Vector<4> result({x, y, z, 1});
    return result;
};

template <std::size_t N>
lin_alg::Vector<3> lin_alg::Vector<N>::CrossProduct(const Vector<3> &rhs, const Vector<3> &lhs)
{
    double x = rhs[1] * lhs[2] - rhs[2] * lhs[1];
    double y = rhs[2] * lhs[0] - rhs[0] * lhs[2];
    double z = rhs[0] * lhs[1] - rhs[1] * lhs[0];
    Vector<3> result({x, y, z});
    return result;
};

template <std::size_t N>
lin_alg::Vector<3> lin_alg::Vector<N>::GetAsVector3()
{
    if (N >= 3)
    {
        double newValues[] = {values[0], values[1], values[2]};
        return Vector<3>(newValues);
    }

    double newValues[3];

    for (std::size_t i = 0; i < N; ++i)
    {
        newValues[i] = values[i];
    }

    for (std::size_t i = N; i < 3; ++i)
    {
        newValues[i] = 0;
    }

    return Vector<3>(newValues);
}

template class lin_alg::Vector<4>;
template class lin_alg::Vector<3>;