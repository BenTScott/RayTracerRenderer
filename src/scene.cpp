#include "scene.h"
#include "rgbimage.h"
#include <iostream>

void Scene::AddObject(SceneObject *object)
{
    objects.push_back(object);
};

void Scene::AddLightSource(Light *light)
{
    light_sources.push_back(light);
};

bool Scene::InShadow(Ray &lightray)
{
    for (std::vector<SceneObject *>::iterator object_iterator = objects.begin(); object_iterator < objects.end(); object_iterator++)
    {
        if ((*object_iterator)->Intersect(lightray))
        {
            return true;
        }
    }
    return false;
};

lin_alg::Vector<3> Scene::CalculateColour(Ray &ray, RayIntersect &intersect)
{
    double total_diffuse_component = 0;
    double total_specular_component = 0;

    for (std::vector<Light *>::iterator light_iterator = light_sources.begin(); light_iterator < light_sources.end(); light_iterator++)
    {
        // Correct for any flt pt error to prevent self shadows
        lin_alg::Vector<3> pos = ray.Position(intersect.t - 0.001);
        Ray lightray = (*light_iterator)->GetLightRay(pos);
        // bool shaded =  InShadow(lightray);
        // double value = intersect.colour[2];
        if (!InShadow(lightray))
        {
            double diffuse_component = lightray.direction.DotProduct(intersect.normal) * (*light_iterator)->intensity;

            //Bling-Phong Specular Term
            lin_alg::Vector<3> h = ray.direction.Scale(-1) + lightray.direction;
            h.Normalise();

            double specular = intersect.object->specular_component * std::pow(h.DotProduct(intersect.normal), 100) * (*light_iterator)->intensity;

            total_diffuse_component += std::max(diffuse_component, 0.0);
            total_specular_component += specular;
        }
    }

    double colour_scale = std::min(ambient_intensity + total_diffuse_component, 1.0);

    lin_alg::Vector<3> white({1, 1, 1});

    return intersect.colour.Scale(colour_scale) + white.Scale(total_specular_component);
};

void Scene::Render(const char *filename, unsigned resolution_width, unsigned resolution_height)
{
    cam.InitialiseResolution(resolution_width, resolution_height);
    RGBImage image(resolution_width, resolution_height);

    for (unsigned i = 0; i < resolution_width; ++i)
    {
        for (unsigned j = 0; j < resolution_height; ++j)
        {
            //double t = INFINITY;
            std::shared_ptr<RayIntersect> closest = nullptr;

            Ray ray = cam.GetRay(i, j);

            for (std::vector<SceneObject *>::iterator object_iterator = objects.begin(); object_iterator < objects.end(); object_iterator++)
            {
                std::shared_ptr<RayIntersect> intersect = (*object_iterator)->Intersect(ray);
                if (intersect && (!closest || intersect->t < closest->t))
                {
                    closest = intersect;
                };
            }

            lin_alg::Vector<3> colour = closest ? CalculateColour(ray, *closest) : background;

            image.SetPixel(i, j, colour);
        }
    }

    image.Encode(filename);
};