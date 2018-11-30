#ifndef INCLUDE_POINTLIGHT_H_
#define INCLUDE_POINTLIGHT_H_

#include <random>
#include "vector.h"
#include "light.h"

class PointLight : public Light
{
  public:
    PointLight(){};
    PointLight(PointLight &obj) : Light(obj.intensity)
    {
        point = obj.point;
    };
    ~PointLight(){};

    PointLight(lin_alg::Vector<3> point, double intensity)
        : Light(intensity), point(point)
    {
    };

    PointLight(lin_alg::Vector<3> point, lin_alg::Vector<3> intensity)
        : Light(intensity), point(point)
    {
    };

    virtual Ray GetLightRay(lin_alg::Vector<3> pos) const override
    {
        lin_alg::Vector<3> difference = point-pos;
        return Ray(pos, difference, difference.Magnitude());
    };

    virtual std::vector<PhotonPathRay> GeneratePhotonRays(unsigned number_of_photons) const override
    {
        std::vector<PhotonPathRay> photon_rays;
        photon_rays.reserve(number_of_photons);
        lin_alg::Vector<3> photon_intensity = this->intensity.Scale(1.0/number_of_photons);
        std::mt19937 generator(rand());
        std::uniform_real_distribution<> distribution(-1.0, 1.0);
        for (unsigned i = 0; i < number_of_photons; ++i)
        {
            Ray ray(point, {distribution(generator), distribution(generator), distribution(generator)});
            photon_rays.push_back(PhotonPathRay(ray, photon_intensity));
        }

        return photon_rays;
    };

    lin_alg::Vector<3> point;
};

#endif