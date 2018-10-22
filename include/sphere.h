#ifndef INCLUDE_SPHERE_H_
#define INCLUDE_SPHERE_H_

#include "vector.h"

class Sphere
{
  public:
    Sphere() : centre(), radius(){};
    Sphere(lin_alg::Vector<4> centre) : centre(centre), radius(1){};
    Sphere(double radius) : centre(), radius(radius){};
    Sphere(lin_alg::Vector<4> centre, double radius) : centre(centre), radius(radius){};

    ~Sphere(){};

    lin_alg::Vector<4> centre;
    double radius;
};

#endif