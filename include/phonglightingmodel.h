#ifndef INCLUDE_PHONGLIGHTINGMODEL_H_
#define INCLUDE_PHONGLIGHTINGMODEL_H_

#include "lightingmodel.h"
#include "photonpathray.h"

class PhongLightingModel : public LightingModel
{
  public:
    PhongLightingModel(double ambient_intensity, unsigned specular_dist) : ambient_intensity(ambient_intensity), specular_dist(specular_dist) {};

    virtual ~PhongLightingModel(){};

    virtual lin_alg::Vector<3> GetGlobalLighting(const RayIntersect &intersect) override;
    virtual lin_alg::Vector<3> GetSpecularLighting(const Light &light, const RayIntersect &intersect) override;
    virtual lin_alg::Vector<3> GetDiffuseLighting(const Light &light, const RayIntersect &intersect) override;
    virtual lin_alg::Vector<3> EstimatedPhotonRadiance(Photon photon, const RayIntersect &intersect, lin_alg::Vector<3> view_dir) override;

    virtual Ray GetReflectionRay(const RayIntersect &intersect) override;
    virtual Ray GetRefractionRay(const RayIntersect &intersect) override;

    virtual PhotonPathRay GetRandomPhotonReflection(std::shared_ptr<RayIntersect> intersect, PhotonPathRay incident, Material::PhotonOutcome outcome) override;

    lin_alg::Vector<3> BRDF(lin_alg::Vector<3> incident, lin_alg::Vector<3> reflected, const RayIntersect &intersect);

  private:
    // Defualt values
    double ambient_intensity = 0;
    unsigned specular_dist = 100;
};

#endif