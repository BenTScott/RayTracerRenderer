#ifndef INCLUDE_BASICLIGHTINGMODEL_H_
#define INCLUDE_BASICLIGHTINGMODEL_H_

#include "lightingmodel.h"

class BasicLightingModel : public LightingModel
{
  public:
    BasicLightingModel(double ambient_intensity, unsigned specular_dist) : ambient_intensity(ambient_intensity), specular_dist(specular_dist) {};

    virtual ~BasicLightingModel(){};

    virtual lin_alg::Vector<3> GetGlobalLighting(const RayIntersect &intersect) override;
    virtual lin_alg::Vector<3> GetSpecularLighting(const Light &light, const RayIntersect &intersect) override;
    virtual lin_alg::Vector<3> GetDiffuseLighting(const Light &light, const RayIntersect &intersect) override;

  private:
    // Defualt values
    double ambient_intensity = 0;
    unsigned specular_dist = 100;
};

#endif