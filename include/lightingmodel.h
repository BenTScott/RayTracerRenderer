#ifndef INCLUDE_LIGHTINGMODEL_H_
#define INCLUDE_LIGHTINGMODEL_H_

#include "vector.h"
#include "light.h"
#include "ray.h"
#include "rayintersect.h"
#include <memory>

class LightingModel
{
public:
  virtual ~LightingModel(){};

  virtual lin_alg::Vector<3> GetGlobalLighting(const RayIntersect &intersect) = 0;
  virtual lin_alg::Vector<3> GetSpecularLighting(const Light &light, const RayIntersect &intersect) = 0;
  virtual lin_alg::Vector<3> GetDiffuseLighting(const Light &light, const RayIntersect &intersect) = 0;
  virtual PhotonPathRay GetRandomPhotonReflection(std::shared_ptr<RayIntersect> intersect, PhotonPathRay incident) = 0;
  virtual Ray GetReflectionRay(const RayIntersect &intersect) = 0;
  virtual Ray GetRefractionRay(const RayIntersect &intersect) = 0;
};

#endif