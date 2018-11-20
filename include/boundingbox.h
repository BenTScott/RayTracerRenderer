#ifndef INCLUDE_BOUNDINGBOX_H_
#define INCLUDE_BOUNDINGBOX_H_

#include "mesh.h"

class Octant : public SceneObject
{
  public:
    Octant() : octants(nullptr){};
    Octant(lin_alg::Vector<3> p1, lin_alg::Vector<3> p2) : point1(p1), point2(p2), octants(nullptr){};
    virtual std::shared_ptr<RayIntersect> Intersect(Ray ray) override;

  protected:
    void AssignFaces(Mesh *contained_mesh, std::vector<unsigned> &face_indices, unsigned max_faces_per_octant){

    };

    lin_alg::Vector<3> point1;
    lin_alg::Vector<3> point2;
    std::vector<Octant> *octants;
    std::vector<unsigned> face_indices;
};

class BoundingBox : public Octant
{
  public:
    BoundingBox(Mesh *contained_mesh, unsigned max_faces_per_octant) : contained_mesh(contained_mesh)
    {
        lin_alg::Vector<3> min({INFINITY, INFINITY, INFINITY});
        lin_alg::Vector<3> max({-INFINITY, -INFINITY, -INFINITY});

        for (const auto &vertex : contained_mesh->vertices)
        {
            for (int i = 0; i < 3; ++i)
            {
                if (vertex.pos[i] > max[i])
                {
                    max[i] = vertex.pos[i];
                }
                if (vertex.pos[i] < min[i])
                {
                    min[i] = vertex.pos[i];
                }
            }
        }
        point1 = min;
        point2 = max;

    };

    virtual std::shared_ptr<RayIntersect> Intersect(Ray ray) override;

  protected:
    Mesh *contained_mesh;
};

#endif