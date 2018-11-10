#ifndef INCLUDE_POINTLIGHT_H_
#define INCLUDE_POINTLIGHT_H_

#include "vector.h"
#include "light.h"

class PointLight : public Light
{
  public:
    PointLight(){};
    PointLight(PointLight &obj) : Light(obj.intensity)
    {
        point = obj.point;
    };
    ~PointLight(){};

    PointLight(lin_alg::Vector<3> point, double intensity)
        : Light(intensity), point(point)
    {
    };

    PointLight(lin_alg::Vector<3> point, lin_alg::Vector<3> intensity)
        : Light(intensity), point(point)
    {
    };

    virtual Ray GetLightRay(lin_alg::Vector<3> pos) const override
    {
        lin_alg::Vector<3> difference = point-pos;
        return Ray(pos, difference, difference.Magnitude());
    };

    lin_alg::Vector<3> point;
};

#endif