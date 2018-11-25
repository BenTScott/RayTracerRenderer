#include "face.h"
#include "plane.h"

void Face::CalculateNormal()
{
    lin_alg::Vector<4> v1_v2_diff = vertices[0]->pos - vertices[1]->pos;
    lin_alg::Vector<4> v1_v3_diff = vertices[0]->pos - vertices[2]->pos;
    normal = lin_alg::CrossProduct(v1_v2_diff, v1_v3_diff).GetAsVector3().Normalise();
};

std::shared_ptr<RayIntersect> Face::Intersect(Ray ray)
{
    Plane face_plane(normal, vertices[0]->pos.GetAsVector3());
    std::shared_ptr<RayIntersect> face_intersect = face_plane.Intersect(ray);

    // Ray does not interect face plane i.e. ray is parrallel to the face
    // or intersect is after another face
    if (!face_intersect)
    {
        return nullptr;
    }

    lin_alg::Vector<3> intersect_pos = ray.Position(face_intersect->t);

    lin_alg::Vector<3> vertex_0 = vertices[0]->pos.GetAsVector3();
    lin_alg::Vector<3> vertex_1 = vertices[1]->pos.GetAsVector3();
    lin_alg::Vector<3> vertex_2 = vertices[2]->pos.GetAsVector3();

    // Get vectors between face verticies
    lin_alg::Vector<3> vector1 = vertex_1 - vertex_0;
    lin_alg::Vector<3> vector2 = vertex_2 - vertex_1;
    lin_alg::Vector<3> vector3 = vertex_0 - vertex_2;

    lin_alg::Vector<3> norm1 = CrossProduct(intersect_pos - vertex_0, vector1);
    lin_alg::Vector<3> norm2 = CrossProduct(intersect_pos - vertex_1, vector2);
    lin_alg::Vector<3> norm3 = CrossProduct(intersect_pos - vertex_2, vector3);

    if (norm1.DotProduct(norm2) >= 0 && norm1.DotProduct(norm3) >= 0 && norm2.DotProduct(norm3) >= 0)
    {
        lin_alg::Vector<3> normal;
        if (use_vertex_normals)
        {
            double face_area = lin_alg::Area(vertex_0, vertex_1, vertex_2);
            double t1 = std::max(0.0, std::min(1.0, lin_alg::Area(vertex_1, intersect_pos, vertex_2) / face_area));
            double t2 = std::max(0.0, std::min(1.0, lin_alg::Area(vertex_2, intersect_pos, vertex_0) / face_area));
            double t3 = std::max(0.0, std::min(1.0, 1 - t1 - t2));
            normal = (normals[0].Scale(t1) + normals[1].Scale(t2) + normals[2].Scale(t3)).Normalise();
        }
        else
        {
            normal = this->normal;
        }

        return std::shared_ptr<RayIntersect>(new RayIntersect(ray, face_intersect->t, colour, this, normal));
        return std::shared_ptr<RayIntersect>(new RayIntersect(ray, face_intersect->t, colour, this, normal));
    }

    return nullptr;
};