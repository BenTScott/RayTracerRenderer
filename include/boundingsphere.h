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
    for (auto obj_ptr : contained_objects)
    {
      delete obj_ptr;
    }
  };

  virtual std::shared_ptr<RayIntersect> Intersect(Ray ray) override;

  std::vector<SceneObject *> contained_objects;

private:
  BoundingSphere();
};

#endif