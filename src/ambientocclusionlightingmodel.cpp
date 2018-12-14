#include <random>
#include "ambientocclusionlightingmodel.h"

lin_alg::Vector<3> AmbientOcclusionLightingModel::GetGlobalLighting(const RayIntersect &intersect)
{
    lin_alg::Vector<3> pos = intersect.GetCorrectedPosition();
    double ambient_count = 0;

    for (unsigned i = 0; i < sample_rate; ++i)
    {
        Ray sample_ray(pos, Random::CosineHemisphereVector(intersect.normal), length);
        
        if (!scene.InShadow(sample_ray))
        {
            ambient_count++;
        }
    }
    double ratio = ambient_count / (double)sample_rate;

    return PhongLightingModel::GetGlobalLighting(intersect).Scale(ratio);
}