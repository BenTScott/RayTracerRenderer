#ifndef _INCLUDE_PHOTONMAP_H_
#define _INCLUDE_PHOTONMAP_H_

#include "photon.h"
#include "rayintersect.h"
#include <vector>
#include "lightingmodel.h"
#include <algorithm>

class PhotonMap
{
public:
  PhotonMap(std::vector<Photon *> &photons) : photons(photons), root(photons){};

  PhotonMap(const PhotonMap &obj)
  {
    for (Photon *photon_ptr : obj.photons)
    {
      this->photons.push_back(new Photon(*photon_ptr));
    }
    this->root = Node(photons);
  }

  ~PhotonMap()
  {
    for (Photon *photon_ptr : photons)
    {
      delete photon_ptr;
    }
    photons.clear();
  };

  PhotonMap(const char *filename);

  std::vector<Photon *> LocatePhotons(lin_alg::Vector<3> point, unsigned max, double &radius);
  lin_alg::Vector<3> GetIrradianceEsitimate(const RayIntersect &intersect, unsigned N, bool cone_filter, LightingModel *lighting_model, bool &use_shadow_rays);
  void ScalePhotons(double scale);
  void WriteToFile(const char *filename);

  std::vector<Photon *> photons;

protected:
  class Node
  {
  public:
    Node(std::vector<Photon *> &photons);
    Node() : median(nullptr){};

    void LocatePhotons(std::vector<Photon *> &nearest, lin_alg::Vector<3> &x, unsigned max, double &r);

    Photon *median;
    int split_dim = -1;
    Node *left = nullptr;
    Node *right = nullptr;
  };

  Node root;
};

#endif