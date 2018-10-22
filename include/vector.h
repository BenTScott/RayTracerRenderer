#ifndef INCLUDE_VECTOR_H_
#define INCLUDE_VECTOR_H_

#include <cstddef>
#include <math.h>

namespace lin_alg
{

template <std::size_t N>
class Vector
{
  public:
    Vector() { Intialise(); };
    Vector(double array[N])
    {
        for (std::size_t i = 0; i < N; ++i)
        {
            values[i] = array[i];
        }
    };
    Vector(const Vector<N> &vector)
    {
        for (std::size_t i = 0; i < N; ++i)
        {
            values[i] = vector[i];
        }
    };
    ~Vector(){};

    const double &operator[](int i) const
    {
        return values[i];
    };

    double &operator[](int i)
    {
        return values[i];
    };

    double Magnitude() const;

    void Normalise();

    double DotProduct(const Vector<N> &vector) const;

    Vector<N> CrossProduct(const Vector<N> &vector) const;

  private:
    void Intialise()
    {
        for (std::size_t i = 0; i < N; ++i)
        {
            values[i] = 0;
        }
    };

    double values[N];
};

} // namespace lin_alg

#endif