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

protected:
  //virtual lin_alg::Vector<3> CalculateColourAtIntersect(const RayIntersect &intersect, unsigned depth = 0) const override;

  virtual RGBImage *GetImage(unsigned resolution_width, unsigned resolution_height);

  std::vector<Photon *> TraceLightRays(std::vector<PhotonPathRay> &photon_rays);

  void TracePhotonPath(std::vector<Photon *> &photons, const PhotonPathRay &photon_ray, int depth);

  void GetPhotonOutcome(std::vector<Photon *> &photons, const PhotonPathRay &photon_ray, std::shared_ptr<RayIntersect> intersect, bool is_direct = false, int depth = 1);

  unsigned global_photons;
  lin_alg::Vector<3> sphere_bound_centre;
  double sphere_bound_radius;

  //TODO: ADD CAUSTICS
  // PhotonMap *GetCausticPhotonMap(unsigned photons_per_object)
  // {
  //     double intensity_sum = 0;
  //     for (Light *light : light_sources)
  //     {
  //         intensity_sum += (light->intensity).Magnitude();
  //     }

  //     std::vector<Photon> emitted_photons;
  //     for (SceneObject *obj : objects)
  //     {
  //         if (obj->specular_component > 0)
  //         {
  //             emitted_photons.reserve(emitted_photons.size() + photons_per_object);
  //             for (Light *light : light_sources)
  //             {
  //                 unsigned light_photon_count = std::round((light->intensity.Magnitude() / intensity_sum) * photons_per_object);
  //                 std::vector<Photon> photons = light->EmmitPhotons(light_photon_count);
  //                 emitted_photons.insert(end(emitted_photons), begin(photons), end(photons));
  //             }
  //         }
  //     }
  //     std::cout << emitted_photons.size() << " caustic photons emmitted";

  //     //TODO: Parallise
  //     std::vector<Photon *> traced_photons;
  //     traced_photons.reserve(emitted_photons.size());

  //     for (Photon &photon : emitted_photons)
  //     {
  //         // Do specular reflections and transmissions and return when hitting diffuse
  //         std::vector<Photon *> photons = TracePhoton(photon);
  //         traced_photons.insert(end(traced_photons), begin(photons), end(photons));
  //     }

  //     std::cout << traced_photons.size() << " traced photons stored";

  //     return new PhotonMap(traced_photons);
  // };
};

#endif