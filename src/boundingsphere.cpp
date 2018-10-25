#include <iterator>
#include "boundingsphere.h"
#include "vector.h"

BoundingSphere::BoundingSphere(Mesh *contained_mesh)
{
    contained_mesh = contained_mesh;

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

bool BoundingSphere::Intersect(Ray ray, double &out_t)
{
    if (!Sphere::Intersect(ray, out_t))
    {
        return false;
    }
    return contained_mesh->Intersect(ray, out_t);
};