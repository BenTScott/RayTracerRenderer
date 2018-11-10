#include "sphere.h"

std::shared_ptr<RayIntersect> Sphere::Intersect(Ray ray)
{
    lin_alg::Vector<3> local_ray_start = ray.init_position - centre.GetAsVector3();

    // Calculate the dicriminant
    double b = 2 * (ray.direction.DotProduct(local_ray_start));
    double c = local_ray_start.DotProduct(local_ray_start) - (radius * radius);

    double discriminant = (b * b) - (4 * c);

    if (discriminant <= 0)
    {
        return nullptr;
    }

    double t1 = (-b + sqrt(discriminant)) / 2.0;
    double t2 = (-b - sqrt(discriminant)) / 2.0;

    double closest_t = t1 < t2 && t1 > 0 ? t1 : t2;

    if (closest_t < 0 || closest_t > ray.length)
    {
        return nullptr;
    }

    lin_alg::Vector<3> normal = (ray.Position(closest_t) - centre.GetAsVector3()).Normalise();
    std::shared_ptr<RayIntersect> intersect(new RayIntersect(ray, closest_t, colour, this, normal));

    return intersect;
};