#include "phonglightingmodel.h"
#include "sceneobject.h"
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

    lin_alg::Vector<3> scale = light.intensity.Scale(intersect.material.GetSpecularConstant() * std::pow(h.DotProduct(intersect.normal), specular_dist)).Bound();

    return lin_alg::Vector<3>({1, 1, 1}).PointwiseMultiply(scale);
};

lin_alg::Vector<3> PhongLightingModel::GetDiffuseLighting(const Light &light, const RayIntersect &intersect)
{
    Ray lightray = light.GetLightRay(intersect.GetCorrectedPosition());
    lin_alg::Vector<3> scale = light.intensity.Scale(lightray.direction.DotProduct(intersect.normal)).Bound();

    return intersect.material.GetDiffuseConstant().PointwiseMultiply(scale);
};

PhotonPathRay PhongLightingModel::GetRandomPhotonReflection(std::shared_ptr<RayIntersect> intersect, PhotonPathRay incident)
{
    // Can I pull this out to a BRDF fucntion for ease.
    // Generate random reflection ray
    std::mt19937 generator(rand());
    std::uniform_real_distribution<> distribution(-1.0, 1.0);
    lin_alg::Vector<3> reflected({distribution(generator), distribution(generator), distribution(generator)});
    if (reflected.DotProduct(intersect->normal) < 0)
    {
        reflected = reflected.Scale(-1);
    }
    reflected.Normalise();

    lin_alg::Vector<3> specular_direction = incident.ray.direction - intersect->normal.Scale(2.0 * incident.ray.direction.DotProduct(intersect->normal));

    lin_alg::Vector<3> diffuse = intersect->material.GetDiffuseConstant() * intersect->normal.DotProduct(reflected);

    // Specular highlights are treated as white
    // TODO: Verify this is correct behaviour
    lin_alg::Vector<3> specular({1, 1, 1});
    specular = specular.Scale(intersect->material.GetSpecularConstant() * std::pow(specular_direction.DotProduct(reflected), specular_dist));

    //TODO: Verfiy photon won't get more energy
    return PhotonPathRay(Ray(intersect->GetCorrectedPosition(), reflected), incident.intensity.PointwiseMultiply(diffuse + specular));
};

Ray PhongLightingModel::GetReflectionRay(const RayIntersect &intersect)
{
    lin_alg::Vector<3> reflected_dir = intersect.ray.direction - intersect.normal.Scale(2.0 * intersect.normal.DotProduct(intersect.ray.direction));
    return Ray(intersect.GetCorrectedPosition(), reflected_dir);
};

Ray PhongLightingModel::GetRefractionRay(const RayIntersect &intersect)
{
    double ratio = intersect.ray.medium_refractive_index / intersect.material.GetRefractiveIndex();
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
