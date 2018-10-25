#ifndef INCLUDE_SPHERE_H_
#define INCLUDE_SPHERE_H_

#include <math.h>
#include "vector.h"
#include "sceneobject.h"

class Sphere : virtual public SceneObject
{
public:
  Sphere() : centre(), radius(), colour(){};
  Sphere(lin_alg::Vector<4> centre, double radius, lin_alg::Vector<3> colour) : centre(centre), radius(radius), colour(colour){};
  Sphere(const Sphere &obj)
  {
    centre = obj.centre;
    colour = obj.colour;
    radius = obj.radius;
  };

  ~Sphere(){};

  virtual bool Intersect(Ray ray, double &out_t) override
  {
    lin_alg::Vector<3> local_ray_start = ray.init_position - centre.GetAsVector3();

    // Calculate the dicriminant
    double b = 2 * (ray.direction.DotProduct(local_ray_start));
    double c = local_ray_start.DotProduct(local_ray_start) - (radius * radius);

    double discriminant = (b * b) - (4 * c);

    if (discriminant <= 0)
    {
      return false;
    }

    out_t = (-b + sqrt(discriminant))/2;

    return true;
  };

  lin_alg::Vector<4> centre;
  double radius;
  lin_alg::Vector<3> colour;
};

#endif