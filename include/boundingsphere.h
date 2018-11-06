#ifndef INCLUDE_BOUNDINGSPHERE_H_
#define INCLUDE_BOUNDINGSPHERE_H_

#include <vector>
#include "sphere.h"
#include "mesh.h"

class BoundingSphere : public Sphere
{
public:
  BoundingSphere(Mesh *contained_mesh);
  BoundingSphere(Sphere *sphere1, Sphere *sphere2);

  ~BoundingSphere()
  {
    for (std::vector<SceneObject *>::iterator it = contained_objects.begin(); it < contained_objects.end(); it++)
    {
      delete *it;
    }
  };

  virtual std::shared_ptr<RayIntersect> Intersect(Ray ray) override;

  std::vector<SceneObject *> contained_objects;

private:
  BoundingSphere();
};

#endif