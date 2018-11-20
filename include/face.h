#ifndef INCLUDE_FACE_H_
#define INCLUDE_FACE_H_

#include "vector.h"
#include "vertex.h"
#include "sceneobject.h"

class Face : public SceneObject
{
public:
  Face(){};
  Face(Vertex* v1, Vertex* v2, Vertex* v3) : vertices{v1, v2, v3} {CalculateNormal();};

  ~Face(){};

  virtual std::shared_ptr<RayIntersect> Intersect(Ray ray) override;

  void CalculateNormal();

  Vertex* vertices[3];
  lin_alg::Vector<3> normal;
  lin_alg::Vector<3> colour;
};

#endif