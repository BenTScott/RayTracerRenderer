#include "2dshapes.h"

#include "plane.h"

void Rectangle::Initialise(lin_alg::Vector<3> p1, lin_alg::Vector<3> p2, lin_alg::Vector<3> p3, lin_alg::Vector<3> colour)
{
    lin_alg::Vector<3> vector = p3 - p2;
    lin_alg::Vector<3> p4 = p1 + vector;
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

    RecalculateNormals();
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
};

std::vector<SurfacePoint> Circle::GetRandomPoints(unsigned samples) const
{
    std::vector<SurfacePoint> points;
    points.reserve(samples);

    std::uniform_real_distribution<> distribution(-radius, radius);

    auto u = lin_alg::Vector<3>({normal[1], -normal[0], 0}).Normalise();
    auto v = lin_alg::CrossProduct(u, normal);

    while (points.size() < samples)
    {
        lin_alg::Vector<3> trial_point = centre + u.Scale(distribution(Random::Generator())) + v.Scale(distribution(Random::Generator()));
        if ((centre - trial_point).Magnitude() <= radius)
        {
            points.push_back(SurfacePoint(trial_point, normal));
        }
    }

    return points;
};
