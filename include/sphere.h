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

  virtual std::shared_ptr<RayIntersect> Intersect(Ray ray) override;

  lin_alg::Vector<4> centre;
  double radius;
  lin_alg::Vector<3> colour;
};

#endif