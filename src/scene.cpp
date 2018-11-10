#include "scene.h"
#include "rgbimage.h"
#include <iostream>
#include <random>

void Scene::AddObject(SceneObject *object)
{
    objects.push_back(object);
};

void Scene::AddLightSource(Light *light)
{
    light_sources.push_back(light);
};

bool Scene::InShadow(Ray &lightray) const
{
    for (std::vector<SceneObject *>::const_iterator object_iterator = objects.begin(); object_iterator < objects.end(); object_iterator++)
    {
        if ((*object_iterator)->Intersect(lightray))
        {
            return true;
        }
    }
    return false;
};

lin_alg::Vector<3> Scene::CalculateColourAtIntersect(RayIntersect &intersect)
{
    lin_alg::Vector<3> colour;

    for (std::vector<Light *>::iterator light_iterator = light_sources.begin(); light_iterator < light_sources.end(); light_iterator++)
    {
        lin_alg::Vector<3> pos = intersect.GetCorrectedPosition();
        Ray lightray = (*light_iterator)->GetLightRay(pos);

        if (!InShadow(lightray))
        {
            colour += lighting_model->GetDiffuseLighting(**light_iterator, intersect);
            colour += lighting_model->GetSpecularLighting(**light_iterator, intersect);
        }
        colour += lighting_model->GetGlobalLighting(intersect);
    }

    return colour;
};

void Scene::Render(const char *filename, unsigned resolution_width, unsigned resolution_height)
{
    cam.InitialiseResolution(resolution_width, resolution_height);
    RGBImage image(resolution_width, resolution_height);

    for (unsigned i = 0; i < resolution_width; ++i)
    {
        for (unsigned j = 0; j < resolution_height; ++j)
        {
            std::shared_ptr<RayIntersect> closest = nullptr;

            Ray ray = cam.GetRay(i, j);

            image.SetPixel(i, j, GetColour(ray));
        }
    }

    image.Encode(filename);
};

lin_alg::Vector<3> Scene::GetColour(Ray &ray)
{
    std::shared_ptr<RayIntersect> closest = nullptr;
    for (std::vector<SceneObject *>::iterator object_iterator = objects.begin(); object_iterator < objects.end(); object_iterator++)
    {
        std::shared_ptr<RayIntersect> intersect = (*object_iterator)->Intersect(ray);
        if (intersect && (!closest || intersect->t < closest->t))
        {
            closest = intersect;
        };
    }

    return closest ? CalculateColourAtIntersect(*closest).Bound() : background;
};

// double Scene::GetAmbientOcclusion(RayIntersect &intersect)
// {
//     lin_alg::Vector<3> pos = intersect.ray.Position(intersect.t - 0.001);
//     double ambient_count = 0;
//     std::mt19937 generator(rand());
//     std::uniform_real_distribution<> distribution(-1.0, 1.0);
//     for (unsigned i = 0; i < ambient_occlusion_sample_rate; ++i)
//     {
//         lin_alg::Vector<3> dir({distribution(generator), distribution(generator), distribution(generator)});
//         if (dir.DotProduct(intersect.normal) < 0)
//         {
//             dir = dir.Scale(-1);
//         }

//         Ray sample_ray(pos, dir, ambient_occlusion_length);
        
//         if (!InShadow(sample_ray))
//         {
//             ambient_count++;
//         }
//     }
//     return ambient_count / (double)ambient_occlusion_sample_rate;
// };