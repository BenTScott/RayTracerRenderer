#include "plane.h"

std::shared_ptr<RayIntersect> Plane::Intersect(Ray ray)
{
    // Get one-sided plane intersect only
    if (ray.direction.DotProduct(normal) > 0)
    {
        return nullptr;
    };

    double t = (point - ray.init_position).DotProduct(normal) / normal.DotProduct(ray.direction);

    // Make sure we are in front of the camera
    if (t < 0)
    {
        return nullptr;
    }

    std::shared_ptr<RayIntersect> intersect(new RayIntersect(t, colour, this));
    return intersect;
};