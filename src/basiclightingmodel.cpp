#include "basiclightingmodel.h"
#include "sceneobject.h"

lin_alg::Vector<3> BasicLightingModel::GetGlobalLighting(const RayIntersect &intersect)
{
    return intersect.colour.Scale(ambient_intensity);
};

lin_alg::Vector<3> BasicLightingModel::GetSpecularLighting(const Light &light, const RayIntersect &intersect)
{
    Ray lightray = light.GetLightRay(intersect.GetCorrectedPosition());

    //Bling-Phong Specular Term
    lin_alg::Vector<3> h = intersect.ray.direction.Scale(-1) + lightray.direction;
    h.Normalise();

    lin_alg::Vector<3> scale = light.intensity.Scale(intersect.object->specular_component * std::pow(h.DotProduct(intersect.normal), specular_dist)).Bound();

    return lin_alg::Vector<3>({1,1,1}).PointwiseMultiply(scale);
};

lin_alg::Vector<3> BasicLightingModel::GetDiffuseLighting(const Light &light, const RayIntersect &intersect)
{
    Ray lightray = light.GetLightRay(intersect.GetCorrectedPosition());
    lin_alg::Vector<3> scale = light.intensity.Scale(lightray.direction.DotProduct(intersect.normal)).Bound();
    
    return intersect.colour.PointwiseMultiply(scale);
};