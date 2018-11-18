#ifndef INCLUDE_RAY_H_
#define INCLUDE_RAY_H_

#include "vector.h"

class Ray
{
  public:
    Ray(){};

    Ray(lin_alg::Vector<3> init_position, lin_alg::Vector<3> direction, double length = INFINITY, double medium_refractive_index = 1)
        : init_position(init_position), direction(direction.Normalise()), length(length), medium_refractive_index(medium_refractive_index){};

    Ray(const Ray &obj) : init_position(obj.init_position), direction(obj.direction), length(obj.length), medium_refractive_index(obj.medium_refractive_index) {};

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