#ifndef INCLUDE_PLANE_H_
#define INCLUDE_PLANE_H_

#include "sceneobject.h"
#include "vector.h"

class Plane : public SceneObject
{
  public:
    Plane() : normal({0, 1, 0}), point(), colour({1,1,1}) {};
    Plane(lin_alg::Vector<3> normal, lin_alg::Vector<3> point, lin_alg::Vector<3> colour) : normal(normal), point(point), colour(colour){};
    Plane(lin_alg::Vector<3> normal, lin_alg::Vector<3> point) : normal(normal), point(point), colour({1, 1, 1}){};
    Plane(Plane &obj)
    {
        normal = obj.normal;
        point = obj.point;
        colour = obj.colour;
    };

    virtual std::shared_ptr<RayIntersect> Intersect(Ray ray) override
    {
        if (ray.direction.DotProduct(point) == 0)
        {
            return nullptr;
        };

        double t = (point - ray.init_position).DotProduct(normal) / normal.DotProduct(ray.direction);
        std::shared_ptr<RayIntersect> intersect(new RayIntersect(t, colour, this));
        return intersect;
    };

    lin_alg::Vector<3> normal;
    lin_alg::Vector<3> point;
    lin_alg::Vector<3> colour;
};

#endif