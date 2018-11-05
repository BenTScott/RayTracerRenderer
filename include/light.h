#ifndef INCLUDE_LIGHT_H_
#define INCLUDE_LIGHT_H_

#include "vector.h"
#include "ray.h"

class Light
{
  public:
    Light(){};
    Light(Light &obj)
    {
        intensity = obj.intensity;
    };
    virtual ~Light(){};

    Light(double intensity)
        : intensity(intensity)
    {
        if (intensity > 1)
        {
            Light::intensity = 1;
        }
    };

    virtual Ray GetLightRay(lin_alg::Vector<3> pos) = 0;

    double intensity;
};

#endif