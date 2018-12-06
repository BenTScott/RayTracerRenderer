#ifndef _INCLUDE_AREALIGHT_H_
#define _INCLUDE_AREALIGHT_H_

#include "sceneobject.h"
#include "light.h"

class AreaLight : public Light, public SceneObject
{
  public:
    AreaLight(SceneObject *base_object, lin_alg::Vector<3> intensity) : Light(intensity), SceneObject(intensity),
                                                                        base_object(base_object){};

    // Light overrides
    virtual Ray GetLightRay(lin_alg::Vector<3> pos) const override;
    virtual std::vector<Ray> GetLightRays(lin_alg::Vector<3> pos, unsigned sample_rate) const override;
    virtual std::vector<PhotonPathRay> GeneratePhotonRays(unsigned number_of_photons) const override;

    // Scene Object overrides
    virtual std::vector<SurfacePoint> GetRandomPoints(unsigned samples) const override;
    virtual std::shared_ptr<RayIntersect> Intersect(Ray ray) override;
    
    SceneObject *base_object = nullptr;
};

#endif