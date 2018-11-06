#include <iterator>
#include "boundingsphere.h"
#include "vector.h"

BoundingSphere::BoundingSphere(Mesh *contained_mesh)
{
    contained_objects.push_back(contained_mesh);

    lin_alg::Vector<4> total;
    for (std::vector<Vertex>::iterator vertex_ptr = contained_mesh->vertices.begin(); vertex_ptr < contained_mesh->vertices.end(); vertex_ptr++)
    {
        total += vertex_ptr->pos;
    }

    centre = total.Scale(1.0 / contained_mesh->vertices.size());

    double max = 0;

    for (std::vector<Vertex>::iterator vertex_ptr = contained_mesh->vertices.begin(); vertex_ptr < contained_mesh->vertices.end(); vertex_ptr++)
    {
        double distance = (centre - vertex_ptr->pos).Magnitude();
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
    if (!Sphere::Intersect(ray))
    {
        return nullptr;
    }

    std::shared_ptr<RayIntersect> closest = nullptr;

    for (std::vector<SceneObject *>::iterator it = contained_objects.begin(); it < contained_objects.end(); it++)
    {
        if (closest)
        {
            std::shared_ptr<RayIntersect> intersect = (*it)->Intersect(ray);
            if (intersect && intersect->t < closest->t)
            {
                closest = intersect;
            }
        }
        else
        {
            closest = (*it)->Intersect(ray);
        }
    }

    return closest;
};