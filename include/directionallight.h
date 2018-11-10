#ifndef INCLUDE_DIRECTIONALLIGHT_H_
#define INCLUDE_DIRECTIONALLIGHT_H_

#include "vector.h"
#include "light.h"

class DirectionalLight : public Light
{
  public:
    DirectionalLight(){};
    DirectionalLight(DirectionalLight &obj) : Light(obj.intensity)
    {
        direction = obj.direction;
    };
    ~DirectionalLight(){};

    DirectionalLight(lin_alg::Vector<3> direction, double intensity) : Light(intensity), direction(direction)
    {
        DirectionalLight::direction.Normalise();
    };

    virtual Ray GetLightRay(lin_alg::Vector<3> pos) const override
    {
        return Ray(pos, direction);
    };

    lin_alg::Vector<3> direction;
};

#endif