#ifndef INCLUDE_VECTOR_H_
#define INCLUDE_VECTOR_H_

#include <cstddef>
#include <math.h>
#include <initializer_list>
#include <iterator>
#include <assert.h>
#include <sstream>

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

    bool operator==(const Vector<N> &rhs) const
    {
        bool equal = true;
        for (std::size_t i = 0; i < N; ++i)
        {
            equal = equal && values[i] == rhs[i];

            if (!equal)
            {
                return false;
            }
        }

        return true;
    }

    bool operator!=(const Vector<N> &rhs) const
    {
        return !(*this == rhs);
    }

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

    Vector<N> operator+(const Vector<N> &rhs) const
    {
        Vector<N> result;
        for (std::size_t i = 0; i < N; ++i)
        {
            result[i] = values[i] + rhs[i];
        }
        return result;
    };

    Vector<N> operator-(const Vector<N> &rhs) const
    {
        Vector<N> result;
        for (std::size_t i = 0; i < N; ++i)
        {
            result[i] = values[i] - rhs[i];
        }
        return result;
    };

    Vector<N> PointwiseMultiply(const Vector<N> &rhs) const
    {
        Vector<N> result;
        for (std::size_t i = 0; i < N; ++i)
        {
            result[i] = values[i] * rhs[i];
        }
        return result;
    };

    double Magnitude() const;

    Vector<N> &Normalise();

    double DotProduct(const Vector<N> &vector) const;

    Vector<3> GetAsVector3() const;

    Vector<N> Scale(double scalar) const
    {
        Vector<N> result;
        for (std::size_t i = 0; i < N; ++i)
        {
            result[i] = values[i] * scalar;
        }
        return result;
    }

    Vector<N> operator*(const double &scalar) const
    {
        Vector<N> result;
        for (std::size_t i = 0; i < N; ++i)
        {
            result[i] = values[i] * scalar;
        }
        return result;
    }

    Vector<N> &Bound(double min = 0.0, double max = 1.0)
    {
        for (std::size_t i = 0; i < N; ++i)
        {
            this->values[i] = std::max(std::min(values[i], max), min);
        }
        return *this;
    }

    double Max()
    {
        double max = 0;
        for (std::size_t i = 0; i < N; ++i)
        {
            max = std::max(max, values[i]);
        }
        return max;
    }

    double Average() const
    {
        double sum = 0;
        for (std::size_t i = 0; i < N; ++i)
        {
            sum += values[i];
        }
        return sum/N;
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

extern Vector<3> CrossProduct(const Vector<3> &rhs, const Vector<3> &lhs);

extern Vector<4> CrossProduct(const Vector<4> &rhs, const Vector<4> &lhs);

extern double Area(const Vector<3> &a, const Vector<3> &b, const Vector<3> &c);

} // namespace lin_alg

#endif