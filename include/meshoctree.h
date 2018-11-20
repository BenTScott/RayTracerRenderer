#ifndef INCLUDE_MESHOCTREE_H
#define INCLUDE_MESHOCTREE_H

#include "mesh.h"
#include "face.h"

class MeshOctree : public SceneObject
{
  public:
    MeshOctree(Mesh *contained_mesh, unsigned max_faces_per_octant) : contained_mesh(contained_mesh)
    {
        lin_alg::Vector<3> min({INFINITY, INFINITY, INFINITY});
        lin_alg::Vector<3> max({-INFINITY, -INFINITY, -INFINITY});

        for (const auto &vertex : contained_mesh->vertices)
        {
            for (int i = 0; i < 3; i++)
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

        node = Node(min, max);
        node.faces = contained_mesh->faces;
        node.Fragment(max_faces_per_octant);
    };

    virtual std::shared_ptr<RayIntersect> Intersect(Ray ray) override
    {
        double t;
        if (node.BoundIntersect(ray, t))
        {
            std::shared_ptr<RayIntersect> intersect = node.Intersect(ray);

            if (intersect)
            {
                intersect->object = contained_mesh;
                return intersect;
            }
        }

        return nullptr;
    };

    class Node : public SceneObject
    {
      public:
        Node() : nodes(nullptr){};
        Node(lin_alg::Vector<3> p1, lin_alg::Vector<3> p2) : p1(p1), p2(p2), nodes(nullptr){};

        bool BoundIntersect(Ray ray, double &t) const
        {
            double t0 = (p1[0] - ray.init_position[0]) / ray.direction[0];
            double t1 = (p2[0] - ray.init_position[0]) / ray.direction[0];
            double t_max = std::max(t0, t1);
            double t_min = std::min(t0, t1);

            for (int i = 1; i < 3; ++i)
            {
                t0 = (p1[i] - ray.init_position[i]) / ray.direction[i];
                t1 = (p2[i] - ray.init_position[i]) / ray.direction[i];

                if (t0 < 0 && t1 < 0)
                {
                    return false;
                }

                // if (t0 < 0 || t1 < 0)
                // {
                //     t = 0;
                //     return true;
                // }
                // else
                // {
                if (t0 < t1)
                {
                    t_min = std::max(t_min, t0);
                    t_max = std::min(t_max, t1);
                }
                else
                {
                    t_min = std::max(t_min, t1);
                    t_max = std::min(t_max, t0);
                }

                if (t_min > t_max)
                {
                    return false;
                }
                //}
            }

            t = t_min >= 0 ? t_min : 0;
            return true;
        };

        virtual std::shared_ptr<RayIntersect> Intersect(Ray ray) override
        {
            if (nodes)
            {
                double bound_intersects[8];
                for (unsigned i = 0; i < nodes->size(); ++i)
                {
                    double t;
                    if ((*nodes)[i].BoundIntersect(ray, t))
                    {
                        bound_intersects[i] = t;
                    }
                    else
                    {
                        bound_intersects[i] = -1;
                    }
                }

                std::shared_ptr<RayIntersect> intersect = nullptr;

                do
                {
                    double min = INFINITY;
                    int index = -1;
                    for (int i = 0; i < 8; ++i)
                    {
                        if (bound_intersects[i] >= 0 && bound_intersects[i] < min)
                        {
                            min = bound_intersects[i];
                            index = i;
                        }
                    }

                    if (index == -1)
                    {
                        return nullptr;
                    }
                    else
                    {
                        bound_intersects[index] = -1;
                        intersect = (*nodes)[index].Intersect(ray);
                    }

                } while (!intersect);

                return intersect;
            }

            std::shared_ptr<RayIntersect> closest = nullptr;

            for (Face &face : faces)
            {
                std::shared_ptr<RayIntersect> intersect = face.Intersect(ray);
                if (!closest || (intersect && intersect->t < closest->t))
                {
                    closest = intersect;
                }
            }

            return closest;
        };

        void Fragment(unsigned max_faces)
        {
            if (faces.size() > max_faces)
            {
                lin_alg::Vector<3> diff = p2 - p1;
                lin_alg::Vector<3> mid_x = lin_alg::Vector<3>({diff[0] / 2.0, 0, 0});
                lin_alg::Vector<3> mid_y = lin_alg::Vector<3>({0, diff[1] / 2.0, 0});
                lin_alg::Vector<3> mid_z = lin_alg::Vector<3>({0, 0, diff[2] / 2.0});

                nodes = new std::vector<Node>();
                // nodes->push_back(Node(p1, p2 - mid_x - mid_y - mid_z));
                // nodes->push_back(Node(p1 + mid_x, p2 - mid_y - mid_z));
                // nodes->push_back(Node(p1 + mid_y, p2 - mid_x - mid_z));
                // nodes->push_back(Node(p1 + mid_x + mid_y, p2 - mid_z));
                // nodes->push_back(Node(p1 + mid_z, p2 - mid_x - mid_y));
                // nodes->push_back(Node(p1 + mid_x + mid_z, p2 - mid_y));
                // nodes->push_back(Node(p1 + mid_y + mid_z, p2 - mid_x));
                // nodes->push_back(Node(p1 + mid_x + mid_y + mid_z, p2));

                nodes->push_back(Node(p1, p1 + diff.Scale(0.5)));
                nodes->push_back(Node(p1 + mid_x, p1 + diff.Scale(0.5) + mid_x));
                nodes->push_back(Node(p1 + mid_y, p1 + diff.Scale(0.5) + mid_y));
                nodes->push_back(Node(p1 + mid_x + mid_y, p1 + diff.Scale(0.5) + mid_x + mid_y));
                nodes->push_back(Node(p1 + mid_z, p1 + diff.Scale(0.5) + mid_z));
                nodes->push_back(Node(p1 + mid_x + mid_z, p1 + diff.Scale(0.5) + mid_x + mid_z));
                nodes->push_back(Node(p1 + mid_y + mid_z, p1 + diff.Scale(0.5) + mid_y + mid_z));
                nodes->push_back(Node(p1 + mid_x + mid_y + mid_z, p1 + diff.Scale(0.5) + mid_x + mid_z + mid_y));

                for (const auto &face : faces)
                {
                    for (auto &node : *nodes)
                    {
                        node.TryAdd(face);
                    }
                }

                faces.clear();

                for (auto &node : *nodes)
                {
                    node.Fragment(max_faces);
                }
            }
        };

        void TryAdd(const Face &face)
        {
            for (int i = 0; i < 3; ++i)
            {
                bool contained = true;
                for (int j = 0; j < 3; ++j)
                {
                    if (face.vertices[i]->pos[j] < p1[j] || face.vertices[i]->pos[j] > p2[j])
                    {
                        contained = false;
                    }
                }
                if (contained)
                {
                    faces.push_back(face);
                    return;
                }
            }
        }

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