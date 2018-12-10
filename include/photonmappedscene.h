#ifndef _INCLUDE_PHOTONMAPPEDSCENE_H_
#define _INCLUDE_PHOTONMAPPEDSCENE_H_

#define M_PI 3.14159265358979323846

#include "scene.h"
#include "photonmap.h"
#include "photonpathray.h"

class PhotonMappedScene : public Scene
{
public:
  PhotonMappedScene(Camera cam, lin_alg::Vector<3> background_colour, unsigned global_photons, lin_alg::Vector<3> sphere_bound_centre, double sphere_bound_radius)
      : Scene(cam, background_colour), global_photons(global_photons), sphere_bound_centre(sphere_bound_centre), sphere_bound_radius(sphere_bound_radius){};

  virtual ~PhotonMappedScene(){};

  PhotonMap *GetGlobalPhotonMap(unsigned number_of_photons);
  PhotonMap *GetCausticPhotonMap(unsigned number_of_photons);

protected:
  //virtual lin_alg::Vector<3> CalculateColourAtIntersect(const RayIntersect &intersect, unsigned depth = 0) const override;

  virtual RGBImage *GetImage(unsigned resolution_width, unsigned resolution_height);

  std::vector<Photon *> TraceLightRays(std::vector<PhotonPathRay> &photon_rays);

  void GetPhotonOutcome(std::vector<Photon *> &photons, const PhotonPathRay &photon_ray, std::shared_ptr<RayIntersect> intersect, bool is_direct = false, int depth = 1);
  void GetCausticPhotonOutcome(std::vector<Photon *> &photons, const PhotonPathRay &photon_ray, std::shared_ptr<RayIntersect> intersect, int depth = 1);

  unsigned global_photons;
  lin_alg::Vector<3> sphere_bound_centre;
  double sphere_bound_radius;
};

#endif