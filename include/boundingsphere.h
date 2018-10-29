#ifndef INCLUDE_BOUNDINGSPHERE_H_
#define INCLUDE_BOUNDINGSPHERE_H_

#include "sphere.h"
#include "mesh.h"

class BoundingSphere : public Sphere
{
public:
  BoundingSphere(Mesh *contained_mesh);

  virtual std::shared_ptr<RayIntersect> Intersect(Ray ray) override;

  Mesh *contained_mesh;
private:
  BoundingSphere();
};

#endif