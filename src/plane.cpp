#include "plane.h"

std::shared_ptr<RayIntersect> Plane::Intersect(Ray ray)
{
    // Get one-sided plane intersect only
    if (ray.direction.DotProduct(normal) > 0)
    {
        return nullptr;
    };

    double t = (point - ray.init_position).DotProduct(normal) / normal.DotProduct(ray.direction);

    if (t < 0 || t > ray.length)
    {
        return nullptr;
    }

    std::shared_ptr<RayIntersect> intersect(new RayIntersect(t, colour, this, normal));
    return intersect;
};