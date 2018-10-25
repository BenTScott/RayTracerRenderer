#ifndef INCLUDE_VECTOR_H_
#define INCLUDE_VECTOR_H_

#include <cstddef>
#include <math.h>
#include <initializer_list>
#include <iterator>
#include <assert.h>

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

    Vector(std::initializer_list<double> list)
    {
        assert(list.size() == N);
        int index = 0;
        for (std::initializer_list<double>::iterator iter = list.begin(); iter < list.end(); iter++)
        {
            values[index++] = *iter;
        }
        // Loop over list.
    };

    ~Vector(){};

    double operator[](int i) const
    {
        return values[i];
    };

    double &operator[](int i)
    {
        return values[i];
    };

    Vector<N> &operator+=(const Vector<N> &rhs)
    {
        for (std::size_t i = 0; i < N; ++i)
        {
            values[i] += rhs[i];
        }

        return *this;
    }

    Vector<N> &operator-=(const Vector<N> &rhs)
    {
        for (std::size_t i = 0; i < N; ++i)
        {
            values[i] -= rhs[i];
        }

        return *this;
    }

    Vector<N> operator+(const Vector<N> &rhs)
    {
        Vector<N> result;
        for (std::size_t i = 0; i < N; ++i)
        {
            result[i] = values[i] + rhs[i];
        }
        return result;
    };

    Vector<N> operator-(const Vector<N> &rhs)
    {
        Vector<N> result;
        for (std::size_t i = 0; i < N; ++i)
        {
            result[i] = values[i] - rhs[i];
        }
        return result;
    };

    double Magnitude() const;

    void Normalise();

    double DotProduct(const Vector<N> &vector) const;

    static Vector<3> CrossProduct(const Vector<3> &rhs, const Vector<3> &lhs);

    static Vector<4> CrossProduct(const Vector<4> &rhs, const Vector<4> &lhs);

    Vector<3> GetAsVector3();

    Vector<N> Scale(double scalar)
    {
        Vector<N> result;
        for (std::size_t i = 0; i < N; ++i)
        {
            result[i] = values[i] * scalar;
        }
        return result;
    }

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