#ifndef INCLUDE_LIGHTINGMODEL_H_
#define INCLUDE_LIGHTINGMODEL_H_

#include "vector.h"
#include "light.h"
#include "ray.h"
#include "rayintersect.h"
#include "photon.h"
#include <memory>

class LightingModel
{
public:
  virtual ~LightingModel(){};

  virtual lin_alg::Vector<3> GetGlobalLighting(const RayIntersect &intersect) = 0;
  virtual lin_alg::Vector<3> GetSpecularLighting(const Light &light, const RayIntersect &intersect) = 0;
  virtual lin_alg::Vector<3> GetDiffuseLighting(const Light &light, const RayIntersect &intersect) = 0;
  virtual lin_alg::Vector<3> EstimatedPhotonRadiance(Photon photon, const RayIntersect &intersect, lin_alg::Vector<3> view_dir) = 0;
  virtual PhotonPathRay GetRandomPhotonReflection(std::shared_ptr<RayIntersect> intersect, PhotonPathRay incident, Material::PhotonOutcome outcome) = 0;
  virtual Ray GetReflectionRay(const RayIntersect &intersect) = 0;
  virtual Ray GetRefractionRay(const RayIntersect &intersect) = 0;
};

#endif