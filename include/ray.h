#ifndef INCLUDE_RAY_H_
#define INCLUDE_RAY_H_

#include "vector.h"

class Ray
{
  public:
    Ray(){};

    Ray(const lin_alg::Vector<3> init_position, const lin_alg::Vector<3> direction, const double length = INFINITY, const double medium_refractive_index = 1)
        : init_position(init_position), direction(direction), length(length), medium_refractive_index(medium_refractive_index)
    {
        this->direction.Normalise();
    };

    Ray(const Ray &obj) : init_position(obj.init_position), direction(obj.direction), length(obj.length), medium_refractive_index(obj.medium_refractive_index){};

    ~Ray(){};

    lin_alg::Vector<3> Position(double t) const
    {
        return init_position + direction.Scale(t);
    };

    lin_alg::Vector<3> init_position;
    lin_alg::Vector<3> direction;
    double length;
    double medium_refractive_index;
};

#endif