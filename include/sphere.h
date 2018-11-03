#ifndef INCLUDE_SPHERE_H_
#define INCLUDE_SPHERE_H_

#include <math.h>
#include "vector.h"
#include "sceneobject.h"

class Sphere : public SceneObject
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

  virtual std::shared_ptr<RayIntersect> Intersect(Ray ray) override
  {
    lin_alg::Vector<3> local_ray_start = ray.init_position - centre.GetAsVector3();

    // Calculate the dicriminant
    double b = 2 * (ray.direction.DotProduct(local_ray_start));
    double c = local_ray_start.DotProduct(local_ray_start) - (radius * radius);

    double discriminant = (b * b) - (4 * c);

    if (discriminant <= 0)
    {
      return nullptr;
    }

    double t1 = (-b + sqrt(discriminant))/2.0;
    double t2 = (-b - sqrt(discriminant))/2.0;

    if (t1 < 0 && t2 < 0)
    {
      return nullptr;
    }

    double closest_t = t1 < t2 && t1 > 0 ? t1 : t2;

    lin_alg::Vector<3> normal = (ray.init_position + ray.direction.Scale(closest_t)) - centre.GetAsVector3();
    normal.Normalise();
    std::shared_ptr<RayIntersect> intersect(new RayIntersect(closest_t, colour, this, normal));

    return intersect;
  };

  lin_alg::Vector<4> centre;
  double radius;
  lin_alg::Vector<3> colour;
};

#endif