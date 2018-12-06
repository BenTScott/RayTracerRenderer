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

MerseneTwisterFactory Random::factory = MerseneTwisterFactory();