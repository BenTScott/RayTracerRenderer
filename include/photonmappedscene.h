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

  void AddSampling(const char *filename, unsigned resolution_width, unsigned resolution_height, unsigned max_samples);

  PhotonMap *GetGlobalPhotonMap(unsigned number_of_photons);
  PhotonMap *GetCausticPhotonMap(unsigned number_of_photons);

  PhotonMap *global_map = nullptr;
  PhotonMap *caustic_map = nullptr;

protected:
  struct PixelTask
  {
    PixelTask(){};
    PixelTask(unsigned x, unsigned y, unsigned sample_rate) : x(x), y(y), sample_rate(sample_rate){};
    unsigned x;
    unsigned y;
    unsigned sample_rate;
  };

  lin_alg::Vector<3> CalculateColourAtIntersect(const RayIntersect &intersect, PhotonMap *photon_map, unsigned depth = 0) const;

  virtual RGBImage *GetImage(unsigned resolution_width, unsigned resolution_height);

  std::vector<Photon *> TraceLightRays(std::vector<PhotonPathRay> &photon_rays);

  void GetPhotonOutcome(std::vector<Photon *> &photons, const PhotonPathRay &photon_ray, std::shared_ptr<RayIntersect> intersect, bool is_direct = false, int depth = 1);
  void GetCausticPhotonOutcome(std::vector<Photon *> &photons, const PhotonPathRay &photon_ray, std::shared_ptr<RayIntersect> intersect, int depth = 1);
  void PixelThreadTask(TaskQueue<PixelTask> &queue, ThreadSafeImage *image, PhotonMap global_map, PhotonMap caustic_map);
  std::vector<PixelTask> GetSampleRates(RGBImage *first_pass);
  void ApplyKernelTransformations(RGBImage *first_pass);

  unsigned global_photons;
  unsigned max_thread;
  unsigned sample_rate;
};

#endif