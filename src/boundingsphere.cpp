#include <iterator>
#include "boundingsphere.h"
#include "vector.h"

BoundingSphere::BoundingSphere(Mesh *contained_mesh)
{
    contained_objects.push_back(contained_mesh);

    lin_alg::Vector<4> total;
    for (const Vertex &vertex : contained_mesh->vertices)
    {
        total += vertex.pos;
    }

    centre = total.Scale(1.0 / contained_mesh->vertices.size());

    double max = 0;

    for (const Vertex &vertex : contained_mesh->vertices)
    {
        double distance = (centre - vertex.pos).Magnitude();
        if (distance > max)
        {
            max = distance;
        }
    }

    radius = max;
};

BoundingSphere::BoundingSphere(Sphere *sphere1, Sphere *sphere2)
{
    centre = (sphere1->centre + sphere2->centre).Scale(0.5);
    radius = (sphere1->centre - sphere2->centre).GetAsVector3().Magnitude() / 2.0 + std::max(sphere1->radius, sphere2->radius);
    contained_objects.push_back(sphere1);
    contained_objects.push_back(sphere2);
}

std::shared_ptr<RayIntersect> BoundingSphere::Intersect(Ray ray)
{
    //Check not in sphere and not intersecting
    double distance_to_centre = (centre.GetAsVector3() - ray.init_position).Magnitude();
    if (distance_to_centre > radius && !Sphere::Intersect(ray))
    {
        return nullptr;
    }

    std::shared_ptr<RayIntersect> closest = nullptr;

    for (const auto obj : contained_objects)
    {
        if (closest)
        {
            std::shared_ptr<RayIntersect> intersect = obj->Intersect(ray);
            if (intersect && intersect->t < closest->t)
            {
                closest = intersect;
            }
        }
        else
        {
            closest = obj->Intersect(ray);
        }
    }

    return closest;
};