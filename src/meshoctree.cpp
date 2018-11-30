#include "meshoctree.h"

MeshOctree::MeshOctree(Mesh *contained_mesh, unsigned max_faces_per_octant) : contained_mesh(contained_mesh)
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

    node = Node(min, max - min);
    node.faces = contained_mesh->faces;
    node.Fragment(max_faces_per_octant);
};

std::shared_ptr<RayIntersect> MeshOctree::Intersect(Ray ray)
{
    double t;
    if (node.BoundIntersect(ray, t))
    {
        std::shared_ptr<RayIntersect> intersect = node.Intersect(ray);

        if (intersect)
        {
            intersect->material = Material(contained_mesh->material).SetDiffuseConstant(intersect->material.GetDiffuseConstant());
            return intersect;
        }
    }

    return nullptr;
};

bool MeshOctree::Node::BoundIntersect(Ray ray, double &t) const
{
    double t_max = INFINITY;
    double t_min = -INFINITY;

    double t0;
    double t1;

    for (int i = 0; i < 3; ++i)
    {
        if (ray.direction[i] == 0) {
            if (ray.init_position[i] < p1[i] || ray.init_position[i] > p2[i])
            {
                return false;
            }
            continue;
        }
        
        // Allow for flt pt errors
        t0 = ((p1[i]-0.0001) - ray.init_position[i]) / ray.direction[i];
        t1 = ((p2[i]+0.0001) - ray.init_position[i]) / ray.direction[i];

        if (t0 < 0 && t1 < 0)
        {
            return false;
        }

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

        if (t_min > t_max || t_min > ray.length)
        {
            return false;
        }
    }

    t = t_min >= 0 ? t_min : 0;
    return true;
};

std::shared_ptr<RayIntersect> MeshOctree::Node::Intersect(Ray ray)
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
        if (intersect && (!closest || intersect->t < closest->t))
        {
            closest = intersect;
        }
    }

    return closest;
};

void MeshOctree::Node::Fragment(unsigned max_faces)
{
    if (faces.size() > max_faces)
    {
        lin_alg::Vector<3> diff = p2 - p1;
        lin_alg::Vector<3> mid_x = lin_alg::Vector<3>({diff[0] * 0.5, 0, 0});
        lin_alg::Vector<3> mid_y = lin_alg::Vector<3>({0, diff[1] * 0.5, 0});
        lin_alg::Vector<3> mid_z = lin_alg::Vector<3>({0, 0, diff[2] * 0.5});

        nodes = new std::vector<Node>();

        nodes->push_back(Node(p1, diff.Scale(0.5)));
        nodes->push_back(Node(p1 + mid_x, diff.Scale(0.5)));
        nodes->push_back(Node(p1 + mid_y, diff.Scale(0.5)));
        nodes->push_back(Node(p1 + mid_x + mid_y, diff.Scale(0.5)));
        nodes->push_back(Node(p1 + mid_z, diff.Scale(0.5)));
        nodes->push_back(Node(p1 + mid_x + mid_z, diff.Scale(0.5)));
        nodes->push_back(Node(p1 + mid_y + mid_z, diff.Scale(0.5)));
        nodes->push_back(Node(p1 + diff.Scale(0.5), diff.Scale(0.5)));

        for (const auto &face : faces)
        {
            bool added = false;
            for (auto &node : *nodes)
            {
                if (node.TryAdd(face) && !added)
                {
                    added = true;
                };
            }
            if (!added)
            {
                std::cout << "Error face not added to any octant";
            }
        }

        faces.clear();

        for (auto &node : *nodes)
        {
            if (!node.faces.empty())
            {
                node.Fragment(max_faces);
            }
        }
    }
};

bool MeshOctree::Node::TryAdd(const Face &face)
{
    lin_alg::Vector<3> p1 = face.vertices[0]->pos.GetAsVector3();
    lin_alg::Vector<3> p2 = face.vertices[1]->pos.GetAsVector3();
    lin_alg::Vector<3> p3 = face.vertices[2]->pos.GetAsVector3();
    lin_alg::Vector<3> v1 = p2 - p1;
    lin_alg::Vector<3> v2 = p3 - p2;
    lin_alg::Vector<3> v3 = p1 - p3;

    Ray rays[] = {Ray(p1, v1, v1.Magnitude()), Ray(p2, v2, v2.Magnitude()), Ray(p3, v3, v3.Magnitude())};

    for (int i = 0; i < 3; ++i)
    {
        double t;
        if (BoundIntersect(rays[i], t))
        {
            faces.push_back(face);
            return true;
        }
    }
    return false;
};