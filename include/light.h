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
        : intensity({intensity, intensity, intensity})
    {
        Light::intensity.Bound();
    };

    Light(lin_alg::Vector<3> intensity)
        : intensity(intensity)
    {
        Light::intensity.Bound();
    };

    virtual Ray GetLightRay(lin_alg::Vector<3> pos) const = 0;

    lin_alg::Vector<3> intensity;
};

#endif