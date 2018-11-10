#ifndef INCLUDE_AMBIENTOCCLUSIONLIGHTINGMODEL_H_
#define INCLUDE_AMBIENTOCCLUSIONLIGHTINGMODEL_H_

#include "lightingmodel.h"
#include "scene.h"

class AmbientOcclusionLightingModel : public LightingModel
{
  public:
    AmbientOcclusionLightingModel(double length, unsigned sample_rate, LightingModel *base_model, const Scene &scene) 
        : length(length), sample_rate(sample_rate), base_model(base_model), scene(scene){};

    virtual ~AmbientOcclusionLightingModel()
    {
        delete base_model;
    };

    virtual lin_alg::Vector<3> GetGlobalLighting(const RayIntersect &intersect) override;

    // Pass through other methods to base
    virtual lin_alg::Vector<3> GetSpecularLighting(const Light &light, const RayIntersect &intersect) override
    {
        return base_model->GetSpecularLighting(light, intersect);
    };

    virtual lin_alg::Vector<3> GetDiffuseLighting(const Light &light, const RayIntersect &intersect) override
    {
        return base_model->GetDiffuseLighting(light, intersect);
    };

  private:
    double length;
    unsigned sample_rate;

    LightingModel *base_model;
    const Scene &scene;
};

#endif