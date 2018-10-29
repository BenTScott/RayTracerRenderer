#ifndef INCLUDE_RAYINTERSECT_H_
#define INCLUDE_RAYINTERSECT_H_

#include "ray.h"
#include "vector.h"

class SceneObject;

class RayIntersect
{
  public:
    RayIntersect(){};
    RayIntersect(double t, lin_alg::Vector<3> colour, SceneObject *obj) : t(t), colour(colour), object(obj) {};
    RayIntersect(RayIntersect& obj)
    {
        //ray = obj.ray;
        t = obj.t;
        colour = obj.colour;
        object = obj.object;
    };

    // lin_alg::Vector<3> IntersectPosition()
    // {
    //     return 
    // };

    //Ray *ray;
    double t;
    lin_alg::Vector<3> colour;
    SceneObject* object;
};

#endif