#ifndef INCLUDE_SPHERE_H_
#define INCLUDE_SPHERE_H_

#include <math.h>
#include "vector.h"
#include "sceneobject.h"

class Sphere : public SceneObject
{
public:
  Sphere() : centre(), radius(){};
  Sphere(lin_alg::Vector<4> centre, double radius, lin_alg::Vector<3> colour) : SceneObject(colour), centre(centre), radius(radius){};

  ~Sphere(){};

  virtual std::shared_ptr<RayIntersect> Intersect(Ray ray) override;
  virtual std::vector<SurfacePoint> GetRandomPoints(unsigned samples) const override;

  lin_alg::Vector<4> centre;
  double radius;
};

#endif