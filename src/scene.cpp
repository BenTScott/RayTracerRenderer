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

void Scene::SetLightingModel(LightingModel *lighting_model)
{
    this->lighting_model = lighting_model;
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

lin_alg::Vector<3> Scene::CalculateColourAtIntersect(const RayIntersect &intersect) const
{
    lin_alg::Vector<3> colour;

    for (std::vector<Light *>::const_iterator light_iterator = light_sources.begin(); light_iterator < light_sources.end(); light_iterator++)
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
    GetImage(resolution_width, resolution_height)->Encode(filename);
};

RGBImage *Scene::GetImage(unsigned resolution_width, unsigned resolution_height)
{
    cam.InitialiseResolution(resolution_width, resolution_height);
    RGBImage* image = new RGBImage(resolution_width, resolution_height);

    unsigned total_pixels = resolution_height * resolution_width;
    monitor->Initialise(total_pixels);

    for (unsigned i = 0; i < resolution_width; ++i)
    {
        for (unsigned j = 0; j < resolution_height; ++j)
        {
            if (monitor)
            {
                monitor->Increment();
            }

            std::shared_ptr<RayIntersect> closest = nullptr;

            Ray ray = cam.GetRay(i, j);

            image->SetPixel(i, j, GetColour(ray));
        }
    }

    return image;
}

lin_alg::Vector<3> Scene::GetColour(const Ray &ray) const
{
    std::shared_ptr<RayIntersect> closest = nullptr;
    for (std::vector<SceneObject *>::const_iterator object_iterator = objects.begin(); object_iterator < objects.end(); object_iterator++)
    {
        std::shared_ptr<RayIntersect> intersect = (*object_iterator)->Intersect(ray);
        if (intersect && (!closest || intersect->t < closest->t))
        {
            closest = intersect;
        };
    }

    return closest ? CalculateColourAtIntersect(*closest).Bound() : background;
};

void Scene::AddMonitoring()
{
    this->monitor = new Monitor();
};