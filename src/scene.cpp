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
                double diffuse_intensity = light_sources[0]->direction.DotProduct(closest->normal);
                if (diffuse_intensity < 0)
                {
                    diffuse_intensity = 0;
                }
                colour = closest->colour.Scale(ambient_intensity + diffuse_intensity * light_sources[0]->intensity);
            }

            image.SetPixel(i, j, colour);
        }
    }

    image.Encode(filename);
};