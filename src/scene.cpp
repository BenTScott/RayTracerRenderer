#include "scene.h"
#include "rgbimage.h"
#include <iostream>

void Scene::AddObject(SceneObject *object)
{
    objects.push_back(object);
};

void Scene::Render(const char *filename, unsigned resolution_height, unsigned resolution_width)
{
    cam.InitialiseResolution(resolution_width, resolution_height);
    RGBImage image(resolution_width, resolution_height);

    lin_alg::Vector<3> red({1, 0, 0});

    for (unsigned i = 0; i < resolution_width; ++i)
    {
        for (unsigned j = 0; j < resolution_height; ++j)
        {
            double t = INFINITY;
            lin_alg::Vector<3> colour = background;
            SceneObject *object = nullptr;

            Ray ray = cam.GetRay(i, j);

            for (std::vector<SceneObject *>::iterator object_iterator = objects.begin(); object_iterator < objects.end(); object_iterator++)
            {
                std::shared_ptr<RayIntersect> intersect = (*object_iterator)->Intersect(ray);
                if (intersect)
                {
                    if (intersect->t < t)
                    {
                        object = intersect->object;
                        t = intersect->t;
                        colour = intersect->colour;
                    }
                };
            }

            // look at 23/21
            image.SetPixel(i, j, colour);
        }
    }

    image.Encode(filename);
};