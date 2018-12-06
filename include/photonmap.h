#ifndef _INCLUDE_PHOTONMAP_H_
#define _INCLUDE_PHOTONMAP_H_

#include "photon.h"
#include <vector>
#include <algorithm>

class PhotonMap
{
  public:
    PhotonMap(std::vector<Photon *> &photons) : photons(photons), root(photons)
    {
    };
    
    ~PhotonMap()
    {
        for (Photon *photon_ptr : photons)
        {
            delete photon_ptr;
        }
        photons.clear();
    };

    std::vector<Photon *> LocatePhotons(lin_alg::Vector<3> point, unsigned max, double &radius);
    void ScalePhotons(double scale);
    std::vector<Photon *> photons;

  protected:
    class Node
    {
      public:
        Node(std::vector<Photon *> &photons);

        void LocatePhotons(std::vector<Photon *> &nearest, lin_alg::Vector<3> &x, unsigned max, double &r);

        Photon *median;
        int split_dim = -1;
        Node *left = nullptr;
        Node *right = nullptr;
    };
    
    Node root;
};

#endif