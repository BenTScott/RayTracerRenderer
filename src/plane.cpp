#include "plane.h"

std::shared_ptr<RayIntersect> Plane::Intersect(Ray ray)
{
    // Get two-sided
    if (ray.direction.DotProduct(normal) == 0)
    {
        return nullptr;
    };

    double t = (point - ray.init_position).DotProduct(normal) / normal.DotProduct(ray.direction);

    if (t < 0 || t > ray.length)
    {
        return nullptr;
    }

    std::shared_ptr<RayIntersect> intersect(new RayIntersect(ray, t, material, normal));
    return intersect;
};

std::vector<SurfacePoint> Plane::GetRandomPoints(unsigned samples) const
{
    std::uniform_real_distribution<> distribution(std::numeric_limits<double>::min(), std::numeric_limits<double>::min());

    std::vector<SurfacePoint> points;
    points.resize(samples);

    auto u = lin_alg::Vector<3>({normal[1],-normal[0], 0}).Normalise();
    auto v = lin_alg::CrossProduct(u, normal);

    for (unsigned i = 0; i < samples; ++i)
    {
        lin_alg::Vector<3> random_point = point + u.Scale(distribution(Random::Generator())) + v.Scale(distribution(Random::Generator()));
        points[i] = SurfacePoint(random_point, normal);
    }

    return points;
};