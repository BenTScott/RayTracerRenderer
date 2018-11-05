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

lin_alg::Vector<3> Scene::CalculateColourAtIntersect(Ray &ray, RayIntersect &intersect)
{
    double total_diffuse_component = 0;
    double total_specular_component = 0;

    for (std::vector<Light *>::iterator light_iterator = light_sources.begin(); light_iterator < light_sources.end(); light_iterator++)
    {
        // Correct for any flt pt error to prevent self shadows
        lin_alg::Vector<3> pos = ray.Position(intersect.t - 0.001);
        Ray lightray = (*light_iterator)->GetLightRay(pos);

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
            std::shared_ptr<RayIntersect> closest = nullptr;

            Ray ray = cam.GetRay(i, j);

            image.SetPixel(i, j, GetColour(ray));
        }
    }

    image.Encode(filename);
};

void Scene::Render(const char *filename, unsigned resolution_width, unsigned resolution_height, unsigned sample_rate)
{
    cam.InitialiseResolution(resolution_width, resolution_height);
    RGBImage image(resolution_width, resolution_height);

    for (unsigned i = 0; i < resolution_width; ++i)
    {
        for (unsigned j = 0; j < resolution_height; ++j)
        {
            std::vector<Ray> rays = cam.GetRandomRaySamples(i, j, sample_rate);

            lin_alg::Vector<3> colour({0, 0, 0});

            for (std::vector<Ray>::iterator ray_iterator = rays.begin(); ray_iterator < rays.end(); ray_iterator++)
            {
                colour += GetColour(*ray_iterator);
            }

            lin_alg::Vector<3> av_col = colour.Scale(1.0 / sample_rate);

            // Get average colour
            image.SetPixel(i, j, av_col);
        }
    }

    image.Encode(filename);
};

lin_alg::Vector<3> Scene::BoundColour(const lin_alg::Vector<3> &colour)
{
    lin_alg::Vector<3> bounded_colour;
    for (int i = 0; i < 3; ++i)
    {
        double value = std::min(colour[i], 1.0);
        value = std::max(value, 0.0);
        bounded_colour[i] = value;
    }
    return bounded_colour;
}

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

    return closest ? BoundColour(CalculateColourAtIntersect(ray, *closest)) : background;
};