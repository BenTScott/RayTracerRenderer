#ifndef INCLUDE_PLANE_H_
#define INCLUDE_PLANE_H_

#include "sceneobject.h"
#include "vector.h"

class Plane : public SceneObject
{
  public:
    Plane() : SceneObject(Material({1,1,1})), normal({0, 1, 0}), point() {};
    Plane(lin_alg::Vector<3> normal, lin_alg::Vector<3> point, lin_alg::Vector<3> colour) : SceneObject(colour), normal(normal), point(point){};
    Plane(lin_alg::Vector<3> normal, lin_alg::Vector<3> point) : SceneObject(Material({1, 1, 1})), normal(normal), point(point){};

    virtual std::shared_ptr<RayIntersect> Intersect(Ray ray) override;
    virtual std::vector<SurfacePoint> GetRandomPoints(unsigned samples) const override;

    lin_alg::Vector<3> normal;
    lin_alg::Vector<3> point;
};

#endif