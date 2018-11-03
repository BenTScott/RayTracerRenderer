#ifndef INCLUDE_DIRECTIONALLIGHT_H_
#define INCLUDE_DIRECTIONALLIGHT_H_

#include "vector.h"

class DirectionalLight
{
  public:
    DirectionalLight(){};
    DirectionalLight(DirectionalLight &obj){};
    ~DirectionalLight(){};

    DirectionalLight(lin_alg::Vector<3> direction, double intensity) : direction(direction), intensity(intensity)
    {
        DirectionalLight::direction.Normalise();
        if (intensity > 1)
        {
            DirectionalLight::intensity = 1;
        }
    };

    lin_alg::Vector<3> direction;
    double intensity;
};

#endif