#include "vector.h"

template <std::size_t N>
double Vector<N>::Magnitude() const
{
    double sum = 0;
    for (std::size_t i = 0; i < N; ++i)
    {
        sum += (values[i]) ^ 2;
    }

    return sqrt(sum);
};

template <std::size_t N>
void Vector<N>::Normalise()
{
    double magnitude = this.Magnitude();

    for (std::size_t i = 0; i < N; ++i)
    {
        values[i] = values[i] / magnitude;
    }
};

template <std::size_t N>
double Vector<N>::DotProduct(const Vector<N> &vector) const
{
    double sum = 0;

    for (std::size_t i = 0; i < N; ++i)
    {
        sum += values[i] * vector[i];
    }

    return sum;
}

template <>
Vector<4> Vector<4>::CrossProduct(const Vector<4> &vector) const
{
    double x = values[1]*vector[2] - values[2]*vector[1];
    double y = values[2]*vector[0] - values[0]*vector[2];
    double z = values[0]*vector[1] - values[1]*vector[0];
    double array[] = {x, y, z, 1};
    Vector<4> result(array);
    return result;
}

template <>
Vector<3> Vector<3>::CrossProduct(const Vector<3> &vector) const
{
    double x = values[1]*vector[2] - values[2]*vector[1];
    double y = values[2]*vector[0] - values[0]*vector[2];
    double z = values[0]*vector[1] - values[1]*vector[0];
    double array[] = {x, y, z};
    Vector<3> result(array);
    return result;
}