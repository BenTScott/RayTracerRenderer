#ifndef INCLUDE_LIGHT_H_
#define INCLUDE_LIGHT_H_

#include <vector>
#include "vector.h"
#include "ray.h"
#include "photonpathray.h"

class Light
{
  public:
    Light(){};
    Light(Light &obj)
    {
        intensity = obj.intensity;
    };
    virtual ~Light(){};

    Light(double intensity)
        : intensity({intensity, intensity, intensity})
    {
        Light::intensity.Bound();
    };

    Light(lin_alg::Vector<3> intensity)
        : intensity(intensity)
    {
        Light::intensity.Bound();
    };

    virtual Ray GetLightRay(lin_alg::Vector<3> pos) const = 0;

    virtual std::vector<Ray> GetLightRays(lin_alg::Vector<3> pos, unsigned sample_rate) const
    {
        (void) sample_rate;
        
        std::vector<Ray> rays;
        rays.push_back(GetLightRay(pos));

        return rays;
    };

    virtual std::vector<PhotonPathRay> GeneratePhotonRays(unsigned number_of_photons) const
    {
        (void) number_of_photons;

        // Does nothing to allow no overriding
        return std::vector<PhotonPathRay>();
    };

    virtual std::vector<PhotonPathRay> GeneratePhotonRays(unsigned number_of_photons, SceneObject *obj_ptr) const
    {
        (void) number_of_photons;
        (void) obj_ptr;

        // Does nothing to allow no overriding
        return std::vector<PhotonPathRay>();
    };

    lin_alg::Vector<3> intensity;
};

#endif