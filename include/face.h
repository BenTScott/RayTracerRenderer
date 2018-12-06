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
  Face(Vertex* v1, lin_alg::Vector<3> n1, Vertex* v2, lin_alg::Vector<3> n2, Vertex* v3, lin_alg::Vector<3> n3) : vertices{v1, v2, v3}, normals{n1, n2, n3}, use_vertex_normals(true) {CalculateNormal();};

  ~Face(){};

  virtual std::shared_ptr<RayIntersect> Intersect(Ray ray) override;
  virtual std::vector<SurfacePoint> GetRandomPoints(unsigned samples) const override;

  void CalculateNormal();

  Vertex* vertices[3];
  lin_alg::Vector<3> normal;
  lin_alg::Vector<3> normals[3];
  bool use_vertex_normals = false;
};

#endif