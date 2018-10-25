#ifndef INCLUDE_RAY_H_
#define INCLUDE_RAY_H_

#include "vector.h"

class Ray
{
  public:
    Ray(){};

    Ray(lin_alg::Vector<3> init_position, lin_alg::Vector<3> direction)
        : init_position(init_position), direction(direction)
    {
        Ray::direction.Normalise();
    };

    Ray(const Ray &obj)
    {
        init_position = obj.init_position;
        direction = obj.direction;
    };
    
    ~Ray(){};

    lin_alg::Vector<3> Position(double t)
    {
        return direction.Scale(t);
    };

    lin_alg::Vector<3> init_position;
    lin_alg::Vector<3> direction;
};

#endif