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

    lin_alg::Vector<3> pos = intersect.GetCorrectedPosition();

    for (const auto &light : light_sources)
    {
        Ray lightray = light->GetLightRay(pos);

        if (!InShadow(lightray))
        {
            colour += lighting_model->GetDiffuseLighting(*light, intersect);
            colour += lighting_model->GetSpecularLighting(*light, intersect);
        }
        colour += lighting_model->GetGlobalLighting(intersect);
    }

    if (intersect.object->reflection_constant > 0 && depth < max_reflection_depth)
    {
        lin_alg::Vector<3> reflected_dir = (intersect.ray.direction + intersect.normal.Scale(2 * intersect.normal.DotProduct(intersect.ray.direction))).Scale(-1);
        Ray reflected(pos, reflected_dir);
        colour += GetColour(reflected, depth + 1).Scale(intersect.object->reflection_constant);
    }

    if (intersect.object->refraction_constant > 0 && depth < max_reflection_depth)
    {
        double ratio = intersect.ray.medium_refractive_index / intersect.object->refractive_index;
        double cos_theta_1 = intersect.ray.direction.DotProduct(intersect.normal);
        double cos_theta_2_sqr = 1 - std::pow(ratio, 2) * (1 - (std::pow(cos_theta_1, 2)));
        if (cos_theta_2_sqr < 0)
        {
            lin_alg::Vector<3> reflected_dir = (intersect.ray.direction + intersect.normal.Scale(2 * intersect.normal.DotProduct(intersect.ray.direction))).Scale(-1);
            Ray reflected(pos, reflected_dir, intersect.ray.medium_refractive_index);
            colour += GetColour(reflected, depth + 1).Scale(intersect.object->refraction_constant);
        }
        else
        {
            lin_alg::Vector<3> refracted_dir = intersect.ray.direction.Scale(ratio) + intersect.normal.Scale(ratio * cos_theta_1 - std::sqrt(cos_theta_2_sqr));
            double new_refraction_index = 1;
            if (intersect.ray.medium_refractive_index != intersect.ray.medium_refractive_index)
            {
                new_refraction_index = intersect.object->refractive_index;
            }
            Ray refracted(intersect.ray.Position(intersect.t + 0.001), refracted_dir, INFINITY, new_refraction_index);
            colour += GetColour(refracted, depth + 1).Scale(intersect.object->refraction_constant);
        }
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

            std::shared_ptr<RayIntersect> closest = nullptr;

            Ray ray = cam.GetRay(i, j);

            image->SetPixel(i, j, GetColour(ray));
        }
    }

    return image;
}

lin_alg::Vector<3> Scene::GetColour(const Ray &ray, unsigned depth) const
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

    return closest ? CalculateColourAtIntersect(*closest, depth).Bound() : background;
};

void Scene::AddMonitoring()
{
    this->monitor = new TaskMonitor();
};