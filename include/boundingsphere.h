#ifndef INCLUDE_BOUNDINGSPHERE_H_
#define INCLUDE_BOUNDINGSPHERE_H_

#include "sphere.h"
#include "mesh.h"

class BoundingSphere : public Sphere
{
public:
  BoundingSphere(Mesh *contained_mesh);

  virtual bool Intersect(Ray ray, double &out_t) override;

  Mesh *contained_mesh;
private:
  BoundingSphere();
};

#endif