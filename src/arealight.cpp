#include "arealight.h"

// Light overrides
Ray AreaLight::GetLightRay(lin_alg::Vector<3> pos) const
{
    SurfacePoint point = base_object->GetRandomPoints(1).front();
    lin_alg::Vector<3> diff = point.GetCorrectedPoint() - pos;
    return Ray(pos, diff.Normalise(), diff.Magnitude());
};

std::vector<Ray> AreaLight::GetLightRays(lin_alg::Vector<3> pos, unsigned sample_rate) const
{
    std::vector<Ray> rays;
    rays.reserve(sample_rate);

    std::vector<SurfacePoint> points = base_object->GetRandomPoints(sample_rate);
    for (const SurfacePoint &surface_point : points)
    {
        lin_alg::Vector<3> diff = surface_point.GetCorrectedPoint() - pos;
        rays.push_back(Ray(pos, diff.Normalise(), diff.Magnitude() - 0.1));
    }

    return rays;
};

std::vector<PhotonPathRay> AreaLight::GeneratePhotonRays(unsigned number_of_photons) const
{
    std::vector<PhotonPathRay> rays;
    rays.reserve(number_of_photons);

    std::vector<SurfacePoint> points = base_object->GetRandomPoints(number_of_photons);

    for (const SurfacePoint &surface_point : points)
    {

        Ray ray(surface_point.GetCorrectedPoint(), Random::CosineHemisphereVector(surface_point.normal));
        rays.push_back(PhotonPathRay(ray, intensity));
    }

    return rays;
};

std::vector<PhotonPathRay> AreaLight::GeneratePhotonRays(unsigned number_of_photons, SceneObject *obj_ptr) const
{
    std::vector<PhotonPathRay> photon_rays;
    photon_rays.reserve(number_of_photons);
    std::vector<SurfacePoint> light_points = base_object->GetRandomPoints(number_of_photons);
    std::vector<SurfacePoint> obj_points = obj_ptr->GetRandomPoints(number_of_photons);

    for (unsigned i = 0; i < number_of_photons; ++i)
    {
        lin_alg::Vector<3> direction = obj_points[i].point - light_points[i].point;
        Ray ray(light_points[i].GetCorrectedPoint(), direction);
        photon_rays.push_back(PhotonPathRay(ray, this->intensity));
    }

    return photon_rays;
};

//Scene Object overrides
std::vector<SurfacePoint> AreaLight::GetRandomPoints(unsigned samples) const
{
    return base_object->GetRandomPoints(samples);
};

std::shared_ptr<RayIntersect> AreaLight::Intersect(Ray ray)
{
    std::shared_ptr<RayIntersect> intersect = base_object->Intersect(ray);
    if (intersect)
    {
        intersect->material = Material(intensity).IsEmitter();
    }
    return intersect;
};