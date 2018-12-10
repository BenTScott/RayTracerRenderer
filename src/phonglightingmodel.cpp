#define _USE_MATH_DEFINES

#include <cmath>
#include "phonglightingmodel.h"
#include "sceneobject.h"
#include <iostream>
#include <random>

lin_alg::Vector<3> PhongLightingModel::GetGlobalLighting(const RayIntersect &intersect)
{
    return intersect.material.GetDiffuseConstant().Scale(ambient_intensity);
};

lin_alg::Vector<3> PhongLightingModel::GetSpecularLighting(const Light &light, const RayIntersect &intersect)
{
    Ray lightray = light.GetLightRay(intersect.GetCorrectedPosition());

    //Bling-Phong Specular Term
    lin_alg::Vector<3> h = (intersect.ray.direction.Scale(-1) + lightray.direction).Normalise();

    lin_alg::Vector<3> scale = light.intensity.Scale(intersect.material.GetSpecularConstant() * (specular_dist + 2.0) * 0.5 * M_1_PI * std::pow(h.DotProduct(intersect.normal), specular_dist)).Bound();

    return lin_alg::Vector<3>({1, 1, 1}).PointwiseMultiply(scale);
};

lin_alg::Vector<3> PhongLightingModel::GetDiffuseLighting(const Light &light, const RayIntersect &intersect)
{
    Ray lightray = light.GetLightRay(intersect.GetCorrectedPosition());
    lin_alg::Vector<3> scale = light.intensity.Scale(lightray.direction.DotProduct(intersect.normal) * M_1_PI).Bound();

    return intersect.material.GetDiffuseConstant().PointwiseMultiply(scale);
};

PhotonPathRay PhongLightingModel::GetRandomPhotonReflection(std::shared_ptr<RayIntersect> intersect, PhotonPathRay incident)
{
    lin_alg::Vector<3> reflected; // = Random::CosineHemisphereVector(intersect->normal);

    double ran = Random::Uniform(0.0, intersect->material.GetReflectionProbablity());

    lin_alg::Vector<3> reflected_intensity;

    if (ran < intersect->material.GetSpecularConstant())
    {
        reflected = Random::PhongSpecularDirection(intersect->normal, 10);
        reflected_intensity = incident.intensity.Scale(intersect->material.GetSpecularConstant())
                                  .Scale(1.0 / intersect->material.GetReflectionProbablity());
    }
    else
    {
        reflected = Random::PhongDiffuseDirection(intersect->normal);
        reflected_intensity = incident.intensity
                                  .PointwiseMultiply(intersect->material.GetDiffuseConstant())
                                  .Scale(1.0 / intersect->material.GetReflectionProbablity());
    }

    // //TODO: Verfiy photon won't get more energy
    // lin_alg::Vector<3> reflected_intensity = incident.intensity
    //                                              .PointwiseMultiply(BRDF(incident.ray.direction, reflected, *intersect))
    //                                              .Scale(1.0 / intersect->material.GetReflectionProbablity());

    return PhotonPathRay(Ray(intersect->GetCorrectedPosition(), reflected), reflected_intensity);
};

Ray PhongLightingModel::GetReflectionRay(const RayIntersect &intersect)
{
    lin_alg::Vector<3> reflected_dir = intersect.ray.direction - intersect.normal.Scale(2.0 * intersect.normal.DotProduct(intersect.ray.direction));
    return Ray(intersect.GetCorrectedPosition(), reflected_dir);
};

Ray PhongLightingModel::GetRefractionRay(const RayIntersect &intersect)
{
    double ratio = intersect.ray.medium_refractive_index / intersect.material.GetRefractiveIndex();
    if (intersect.ray.medium_refractive_index == intersect.material.GetRefractiveIndex())
    {
        ratio = intersect.material.GetRefractiveIndex();
    }

    //double ratio = intersect.ray.medium_refractive_index / intersect.material.GetRefractiveIndex();
    double cos_theta_1 = intersect.ray.direction.DotProduct(intersect.normal);
    double cos_theta_2_sqr = 1 - std::pow(ratio, 2) * (1 - (std::pow(cos_theta_1, 2)));

    // Total internal refraction
    if (cos_theta_2_sqr < 0)
    {
        return GetReflectionRay(intersect);
    }

    lin_alg::Vector<3> refracted_dir = intersect.ray.direction.Scale(ratio) + intersect.normal.Scale(ratio * cos_theta_1 - std::sqrt(cos_theta_2_sqr));
    double new_refraction_index = 1;
    if (intersect.ray.medium_refractive_index == new_refraction_index)
    {
        new_refraction_index = intersect.material.GetRefractiveIndex();
    }

    // Correct position into the object (opposite to usual correction)
    return Ray(intersect.GetCorrectedPosition(-0.001), refracted_dir, INFINITY, new_refraction_index);
};

lin_alg::Vector<3> PhongLightingModel::BRDF(lin_alg::Vector<3> incident, lin_alg::Vector<3> reflected, const RayIntersect &intersect)
{
    lin_alg::Vector<3> specular_direction = incident - intersect.normal.Scale(2.0 * incident.DotProduct(intersect.normal));

    lin_alg::Vector<3> diffuse = intersect.material.GetDiffuseConstant() * intersect.normal.DotProduct(reflected) * M_1_PI;

    // Specular highlights are treated as white
    lin_alg::Vector<3> specular({1, 1, 1});
    specular = specular.Scale(intersect.material.GetSpecularConstant() * (specular_dist + 2.0) * 0.5 * M_1_PI * std::pow(specular_direction.DotProduct(reflected), specular_dist));

    return specular + diffuse;
};

lin_alg::Vector<3> PhongLightingModel::EstimatedPhotonRadiance(Photon photon, const RayIntersect &intersect, lin_alg::Vector<3> view_dir)
{
    return BRDF(photon.direction, view_dir.Normalise(), intersect).PointwiseMultiply(photon.intensity);
};