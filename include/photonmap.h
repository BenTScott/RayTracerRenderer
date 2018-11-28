#ifndef _INCLUDE_PHOTONMAP_H_
#define _INCLUDE_PHOTONMAP_H_

#include "photon.h"
#include <vector>
#include <algorithm>

class PhotonMap
{
  public:
    PhotonMap(std::vector<Photon *> photons) : photons(photons), root(photons){};

    class Node
    {
      public:
        Node(std::vector<Photon *> photons)
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
                left = new Node(s1);
                right = new Node(s2);
            }
            else
            {
                median = photons[0];
            }
        };

        void LocatePhotons(std::vector<Photon *> &nearest, lin_alg::Vector<3> &x, double &d)
        {
            if (left && right)
            {
                double plane_distance = median->position[split_dim] - x[split_dim];
                if (plane_distance < 0)
                {
                    left->LocatePhotons(nearest, x, d);
                    if (std::pow(plane_distance, 2) < std::pow(d, 2))
                    {
                        right->LocatePhotons(nearest, x, d);
                    }
                }
                else
                {
                    right->LocatePhotons(nearest, x, d);
                    if (std::pow(plane_distance, 2) < std::pow(d, 2))
                    {
                        left->LocatePhotons(nearest, x, d);
                    }
                }
            }

            double true_distance_sqr = (median->position - x).Magnitude();
            if (true_distance_sqr < d)
            {
                nearest.push_back(median);
                std::push_heap(
                    nearest.begin(),
                    nearest.end(),
                    [=](Photon *a, Photon *b) {
                        return (a->position - x).Magnitude() < (b->position - x).Magnitude();
                    });
            }
        }

        Photon *median;
        int split_dim = -1;
        Node *left = nullptr;
        Node *right = nullptr;
    };

    std::vector<Photon *> LocatePhotons(lin_alg::Vector<3> point, double d)
    {
        std::vector<Photon *> nearest;
        root.LocatePhotons(nearest, point, d);
        return nearest;
    };

    std::vector<Photon *> photons;
    Node root;
};

#endif