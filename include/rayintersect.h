#ifndef INCLUDE_RAYINTERSECT_H_
#define INCLUDE_RAYINTERSECT_H_

#include "ray.h"
#include "vector.h"
#include "material.h"

class SceneObject;

class RayIntersect
{
  public:
    RayIntersect(){};
    RayIntersect(Ray ray, double t, Material material, lin_alg::Vector<3> normal) : ray(ray), t(t), material(material), normal(normal){};
    //RayIntersect(RayIntersect &obj) : ray(obj.ray), t(obj.t), colour(obj.colour), object(obj.object){};

    lin_alg::Vector<3> Position() const
    {
        return ray.Position(t);
    };

    lin_alg::Vector<3> GetCorrectedPosition(double error = 0.001) const
    {
        return ray.Position(t - error);
    };

    Ray ray;
    double t;
    Material material;
    lin_alg::Vector<3> normal;
};

#endif