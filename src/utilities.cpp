#define _USE_MATH_DEFINES

#include <cmath>
#include "utilities.h"

lin_alg::Vector<3> Random::RandomUnitVector()
{
    std::uniform_real_distribution<> distribution(-1.0, 1.0);

    lin_alg::Vector<3> direction;
    do
    {
        direction = {distribution(factory.generator), distribution(factory.generator), distribution(factory.generator)};
    } while (direction.Magnitude() > 1);

    return direction.Normalise();
};

lin_alg::Vector<3> Random::CosineHemisphereVector(lin_alg::Vector<3> normal)
{
    auto u = lin_alg::CrossProduct(fabs(normal[0])>0.1?lin_alg::Vector<3>({0, 1, 0}):lin_alg::Vector<3> ({1,0,0}), normal).Normalise();
    //auto u = lin_alg::Vector<3>({normal[1], -normal[0], 0}).Normalise();
    auto v = lin_alg::CrossProduct(normal, u);
    std::uniform_real_distribution<> distribution(0.0, 1.0);
    double zeta1 = distribution(factory.generator);
    double zeta2= distribution(factory.generator);;

    lin_alg::Vector<3> dir = u.Scale(cos(2*M_PI*zeta2)*sqrt(zeta1)) + normal.Scale(sqrt(1 -zeta1)) + v.Scale(sin(2*M_PI*zeta2)*sqrt(zeta1));

    return dir.Normalise();
};

MerseneTwisterFactory Random::factory = MerseneTwisterFactory();