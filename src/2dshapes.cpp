#include "2dshapes.h"

#include "plane.h"

void Rectangle::Initialise(lin_alg::Vector<3> p1, lin_alg::Vector<3> p2,lin_alg::Vector<3> p3, lin_alg::Vector<3> colour)
{
    lin_alg::Vector<3> vector1 = p1 - p2;
    lin_alg::Vector<3> vector2 = p3 - p2;
    lin_alg::Vector<3> p4 = p1 + vector1 + vector2;
    lin_alg::Vector<3> points[] = {p1, p2, p3, p4};

    for (int i = 0; i < 4; ++i)
    {
        Vertex v(points[i]);
        vertices.push_back(v);
    }

    Face f1(&vertices[0], &vertices[1], &vertices[2]);
    Face f2(&vertices[0], &vertices[2], &vertices[3]);

    faces.push_back(f1);
    faces.push_back(f2);

    vertices[0].faces.push_back(&faces[0]);
    vertices[0].faces.push_back(&faces[1]);
    vertices[2].faces.push_back(&faces[0]);
    vertices[2].faces.push_back(&faces[1]);
    vertices[1].faces.push_back(&faces[0]);
    vertices[3].faces.push_back(&faces[1]);

    SetColour(colour);
};

std::shared_ptr<RayIntersect> Rectangle::Intersect(Ray ray)
{
    return Mesh::Intersect(ray);
}

std::shared_ptr<RayIntersect> Circle::Intersect(Ray ray)
{
    Plane plane(normal, centre);
    std::shared_ptr<RayIntersect> plane_intersect = plane.Intersect(ray);

    if ((plane_intersect->Position() - centre).Magnitude() > radius)
    {
        return nullptr;
    }
    else
    {
        plane_intersect->material = this->material;
        return plane_intersect;
    }
}
