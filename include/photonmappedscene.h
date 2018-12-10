#ifndef _INCLUDE_PHOTONMAPPEDSCENE_H_
#define _INCLUDE_PHOTONMAPPEDSCENE_H_

#define M_PI 3.14159265358979323846

#include "scene.h"
#include "photonmap.h"
#include "photonpathray.h"
#include "threadsafeimage.h"
#include "taskqueue.h"

class PhotonMappedScene : public Scene
{
public:
  PhotonMappedScene(Camera cam, lin_alg::Vector<3> background_colour, unsigned global_photons, unsigned max_thread, unsigned sample_rate)
      : Scene(cam, background_colour), global_photons(global_photons), max_thread(max_thread), sample_rate(sample_rate){};

  virtual ~PhotonMappedScene(){};

  PhotonMap *GetGlobalPhotonMap(unsigned number_of_photons);
  PhotonMap *GetCausticPhotonMap(unsigned number_of_photons);

protected:
  struct PixelTask
  {
    PixelTask(){};
    PixelTask(unsigned x, unsigned y) : x(x), y(y){};
    unsigned x;
    unsigned y;
  };

  lin_alg::Vector<3> CalculateColourAtIntersect(const RayIntersect &intersect, PhotonMap *photon_map, unsigned depth = 0) const;

  virtual RGBImage *GetImage(unsigned resolution_width, unsigned resolution_height);

  std::vector<Photon *> TraceLightRays(std::vector<PhotonPathRay> &photon_rays);

  void GetPhotonOutcome(std::vector<Photon *> &photons, const PhotonPathRay &photon_ray, std::shared_ptr<RayIntersect> intersect, bool is_direct = false, int depth = 1);
  void GetCausticPhotonOutcome(std::vector<Photon *> &photons, const PhotonPathRay &photon_ray, std::shared_ptr<RayIntersect> intersect, int depth = 1);
  void PixelThreadTask(TaskQueue<PixelTask> &queue, ThreadSafeImage *image, PhotonMap global_map, PhotonMap caustic_map);

  unsigned global_photons;
  unsigned max_thread;
  unsigned sample_rate;
  // PhotonMap *global_map = nullptr;
  // PhotonMap *caustic_map = nullptr;
};

#endif