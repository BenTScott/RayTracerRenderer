#ifndef _INCLUDE_UTILITIES_H_
#define _INCLUDE_UTILITIES_H_

#define _USE_MATH_DEFINES
 
#include <cmath>
#include <random>
#include <ctime>
#include "vector.h"

// Credited to https://stackoverflow.com/a/6943003
template <typename I>
I RandomElement(I begin, I end)
{
    const unsigned long n = std::distance(begin, end);
    const unsigned long divisor = (RAND_MAX + 1) / n;

    unsigned long k;
    do
    {
        k = std::rand() / divisor;
    } while (k >= n);

    std::advance(begin, k);
    return begin;
};

class MerseneTwisterFactory
{
  public:
    MerseneTwisterFactory()
    {
        seed = rd();
        generator = std::mt19937(seed);
    }

    unsigned seed;
    std::mt19937 generator;

  private:
    std::random_device rd;
};

class Random
{
  public:
    static std::mt19937 &Generator()
    {
        return factory.generator;
    };

    static lin_alg::Vector<3> RandomUnitVector();
    static lin_alg::Vector<3> CosineHemisphereVector(lin_alg::Vector<3> normal);

    static unsigned Seed()
    {
        return factory.seed;
    }

  private:
    static MerseneTwisterFactory factory;
};

#endif