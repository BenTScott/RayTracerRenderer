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
    for (const auto &obj : objects)
    {
        if (obj->Intersect(lightray))
        {
            return true;
        }
    }
    return false;
};

lin_alg::Vector<3> Scene::CalculateColourAtIntersect(const RayIntersect &intersect, unsigned depth) const
{
    lin_alg::Vector<3> colour;

    if (intersect.material.IsEmmitter())
    {
        colour += intersect.material.GetDiffuseConstant();
    }

    lin_alg::Vector<3> pos = intersect.GetCorrectedPosition();

    for (const auto &light : light_sources)
    {
        std::vector<Ray> lightrays = light->GetLightRays(pos, 50);

        for (Ray &ray : lightrays)
        {
            if (!InShadow(ray))
            {
                colour += lighting_model->GetDiffuseLighting(*light, intersect).Scale(1.0 / lightrays.size());
                colour += lighting_model->GetSpecularLighting(*light, intersect).Scale(1.0 / lightrays.size());
            }
        }
        
        colour += lighting_model->GetGlobalLighting(intersect);
    }

    if (intersect.material.GetReflectionConstant() > 0 && depth < max_reflection_depth)
    {
        colour += GetColour(lighting_model->GetReflectionRay(intersect), depth + 1).Scale(intersect.material.GetReflectionConstant());
    }

    if (intersect.material.GetRefractionConstant().Magnitude() > 0 && depth < max_reflection_depth)
    {
        colour += GetColour(lighting_model->GetRefractionRay(intersect), depth + 1).PointwiseMultiply(intersect.material.GetRefractionConstant());
    }

    return colour;
};

void Scene::Render(const char *filename, unsigned resolution_width, unsigned resolution_height)
{
    cam.InitialiseResolution(resolution_width, resolution_height);
    GetImage(resolution_width, resolution_height)->Encode(filename);
};

RGBImage *Scene::GetImage(unsigned resolution_width, unsigned resolution_height)
{
    cam.InitialiseResolution(resolution_width, resolution_height);
    RGBImage *image = new RGBImage(resolution_width, resolution_height);

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

            Ray ray = cam.GetRay(i, j);

            image->SetPixel(i, j, GetColour(ray));
        }
    }

    return image;
}

lin_alg::Vector<3> Scene::GetColour(const Ray &ray, unsigned depth) const
{
    std::shared_ptr<RayIntersect> closest = GetRayIntersect(ray);

    return closest ? CalculateColourAtIntersect(*closest, depth).Bound() : background;
};

std::shared_ptr<RayIntersect> Scene::GetRayIntersect(Ray ray) const
{
    std::shared_ptr<RayIntersect> closest = nullptr;
    for (const auto &obj : objects)
    {
        std::shared_ptr<RayIntersect> intersect = obj->Intersect(ray);
        if (intersect && (!closest || intersect->t < closest->t))
        {
            closest = intersect;
        };
    }

    if (closest->normal.DotProduct(ray.direction) > 0)
    {
        closest->normal = closest->normal * -1;
    }

    return closest;
};

void Scene::AddMonitoring()
{
    this->monitor = new TaskMonitor();
};