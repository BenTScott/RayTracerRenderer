#ifndef INCLUDE_RAY_H_
#define INCLUDE_RAY_H_

#include "vector.h"

class Ray
{
  public:
    Ray(){};

    Ray(lin_alg::Vector<3> init_position, lin_alg::Vector<3> direction, double length = INFINITY)
        : init_position(init_position), direction(direction.Normalise()), length(length){};

    Ray(const Ray &obj)
    {
        init_position = obj.init_position;
        direction = obj.direction;
        length = obj.length;
    };

    ~Ray(){};

    lin_alg::Vector<3> Position(double t) const
    {
        return init_position + direction.Scale(t);
    };

    lin_alg::Vector<3> init_position;
    lin_alg::Vector<3> direction;
    double length;
};

#endif