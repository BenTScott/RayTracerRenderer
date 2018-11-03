#include "scene.h"
#include "rgbimage.h"
#include <iostream>

void Scene::AddObject(SceneObject *object)
{
    objects.push_back(object);
};

void Scene::AddLightSource(DirectionalLight *light)
{
    light_sources.push_back(light);
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

            lin_alg::Vector<3> colour = background;

            if (closest)
            {
                double total_diffuse_component = 0;

                for (std::vector<DirectionalLight *>::iterator light_iterator = light_sources.begin(); light_iterator < light_sources.end(); light_iterator++)
                {
                    double diffuse_component = (*light_iterator)->direction.DotProduct(closest->normal)*(*light_iterator)->intensity;

                    total_diffuse_component += std::max(diffuse_component, 0.0);
                }

                double colour_scale = std::min(ambient_intensity + total_diffuse_component, 1.0);

                colour = closest->colour.Scale(colour_scale);
            }

            image.SetPixel(i, j, colour);
        }
    }

    image.Encode(filename);
};