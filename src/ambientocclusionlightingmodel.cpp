#include <random>
#include "ambientocclusionlightingmodel.h"

lin_alg::Vector<3> AmbientOcclusionLightingModel::GetGlobalLighting(const RayIntersect &intersect)
{
    lin_alg::Vector<3> pos = intersect.GetCorrectedPosition();
    double ambient_count = 0;

    //TODO: Performance increase by factoring out
    std::mt19937 generator(rand());
    std::uniform_real_distribution<> distribution(-1.0, 1.0);
    for (unsigned i = 0; i < sample_rate; ++i)
    {
        lin_alg::Vector<3> dir({distribution(generator), distribution(generator), distribution(generator)});
        if (dir.DotProduct(intersect.normal) < 0)
        {
            dir = dir.Scale(-1);
        }

        Ray sample_ray(pos, dir, length);
        
        if (!scene.InShadow(sample_ray))
        {
            ambient_count++;
        }
    }
    double ratio = ambient_count / (double)sample_rate;

    return PhongLightingModel::GetGlobalLighting(intersect).Scale(ratio);
}