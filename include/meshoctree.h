#ifndef INCLUDE_MESHOCTREE_H
#define INCLUDE_MESHOCTREE_H

#include "mesh.h"
#include "face.h"

class MeshOctree : public SceneObject
{
  public:
    MeshOctree(Mesh *contained_mesh, unsigned max_faces_per_octant);

    virtual std::shared_ptr<RayIntersect> Intersect(Ray ray) override;
    virtual std::vector<SurfacePoint> GetRandomPoints(unsigned samples) const override;

    class Node
    {
      public:
        Node() : nodes(nullptr){};
        Node(lin_alg::Vector<3> p1, lin_alg::Vector<3> v) : p1(p1), p2(p1 + v), nodes(nullptr){};

        bool BoundIntersect(Ray ray, double &t) const;

        std::shared_ptr<RayIntersect> Intersect(Ray ray);

        void Fragment(unsigned max_faces);

        bool TryAdd(const Face &face);

        ~Node()
        {
            delete nodes;
        }

        lin_alg::Vector<3> p1;
        lin_alg::Vector<3> p2;
        std::vector<Node> *nodes;
        std::vector<Face> faces;
    };

    Node node;
    Mesh *contained_mesh;
};

#endif