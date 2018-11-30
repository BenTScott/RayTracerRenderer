#ifndef INCLUDE_AMBIENTOCCLUSIONLIGHTINGMODEL_H_
#define INCLUDE_AMBIENTOCCLUSIONLIGHTINGMODEL_H_

#include "phonglightingmodel.h"
#include "scene.h"

class AmbientOcclusionLightingModel : public PhongLightingModel
{
  public:
    AmbientOcclusionLightingModel(double ambient_intensity, unsigned specular_dist, double length, unsigned sample_rate, const Scene &scene) 
        : PhongLightingModel(ambient_intensity, specular_dist), length(length), sample_rate(sample_rate), scene(scene){};

    virtual lin_alg::Vector<3> GetGlobalLighting(const RayIntersect &intersect) override;

  private:
    double length;
    unsigned sample_rate;
    const Scene &scene;
};

#endif