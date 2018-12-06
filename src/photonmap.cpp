#include "photonmap.h"

PhotonMap::Node::Node(std::vector<Photon *> &photons)
{
    if (photons.size() > 1)
    {
        lin_alg::Vector<3> min({INFINITY, INFINITY, INFINITY});
        lin_alg::Vector<3> max({-INFINITY, -INFINITY, -INFINITY});
        for (const Photon *photon : photons)
        {
            for (int i = 0; i < 3; i++)
            {
                max[i] = std::max(max[i], photon->position[i]);
                min[i] = std::min(min[i], photon->position[i]);
            }
        }

        double dim_diff = -INFINITY;

        for (int i = 0; i < 3; i++)
        {
            if (max[i] - min[i] > dim_diff)
            {
                dim_diff = max[i] - min[i];
                split_dim = i;
            }
        }

        for (const Photon *photon : photons)
        {
            for (int i = 0; i < 3; i++)
            {
                max[i] = std::max(max[i], photon->position[i]);
                min[i] = std::min(min[i], photon->position[i]);
            }
        }

        std::sort(photons.begin(), photons.end(), [=](Photon *a, Photon *b) { return a->position[split_dim] < b->position[split_dim]; });

        std::vector<Photon *> s1;
        std::vector<Photon *> s2;
        auto test1 = photons.size();
        auto test = photons.size() / 2;
        s1.reserve(photons.size() / 2);
        s2.reserve(photons.size() / 2);

        for (size_t i = 0; i < photons.size(); ++i)
        {
            if (i < photons.size() / 2)
            {
                s1.push_back(photons[i]);
            }
            else if (i > photons.size() / 2)
            {
                s2.push_back(photons[i]);
            }
        }

        median = photons[photons.size() / 2];
        double debug = median->position[0];
        if (s1.size() >= 1)
        {
            left = new Node(s1);
        }
        if (s2.size() >= 1)
        {
            right = new Node(s2);
        }
    }
    else
    {
        //double debug = photons[0]->position[0];
        median = photons[0];
    }
};

void PhotonMap::Node::LocatePhotons(std::vector<Photon *> &nearest, lin_alg::Vector<3> &x, unsigned max, double &r)
{
    //double debug = median->position[0];
    if (split_dim >= 0)
    {
        double plane_distance = x[split_dim] - median->position[split_dim];

        if (plane_distance < 0)
        {
            if (left)
            {
                left->LocatePhotons(nearest, x, max, r);
            }
            if (right && std::pow(plane_distance, 2) < std::pow(r, 2))
            {
                right->LocatePhotons(nearest, x, max, r);
            }
        }
        else
        {
            if (right)
            {
                right->LocatePhotons(nearest, x, max, r);
            }
            if (left && std::pow(plane_distance, 2) < std::pow(r, 2))
            {
                left->LocatePhotons(nearest, x, max, r);
            }
        }
    }

    double true_distance = (median->position - x).Magnitude();
    if (true_distance < r)
    {
        nearest.push_back(median);
        std::push_heap(
            nearest.begin(),
            nearest.end(),
            [=](Photon *a, Photon *b) {
                return (a->position - x).Magnitude() < (b->position - x).Magnitude();
            });

        if (nearest.size() > max)
        {
            pop_heap(nearest.begin(), nearest.end(), [&](Photon *a, Photon *b) {
                return (a->position - x).Magnitude() < (b->position - x).Magnitude();
            });
            nearest.pop_back();
            r = (nearest.front()->position - x).Magnitude();
        }
    }
};

std::vector<Photon *> PhotonMap::LocatePhotons(lin_alg::Vector<3> point, unsigned max, double &radius)
{
    std::vector<Photon *> nearest;
    radius = INFINITY;
    root.LocatePhotons(nearest, point, max, radius);
    radius = (nearest.front()->position - point).Magnitude();
    return nearest;
};

void PhotonMap::ScalePhotons(double scalar)
{
    for (auto &photon_ptr : photons)
    {
        photon_ptr->intensity = photon_ptr->intensity.Scale(scalar);
    }
}