#ifndef INCLUDE_SCENE_H_
#define INCLUDE_SCENE_H_

#include <vector>
#include <string>
#include <iterator>
#include "sceneobject.h"
#include "camera.h"
#include "rgbimage.h"

class Scene
{
public:
  Scene(Camera cam) : cam(cam){};

  ~Scene(){};

  void AddObject(SceneObject *object)
  {
    objects.push_back(object);
  };

  // TODO: Normalise these on input...

  void Render(const char *filename, unsigned resolution_height, unsigned resolution_width)
  {
    cam.InitialiseResolution(resolution_width, resolution_height);
    RGBImage image(resolution_width, resolution_height);

    double arr[] = {1, 0, 0};
    lin_alg::Vector<3> black;
    lin_alg::Vector<3> red(arr);

    for (unsigned i = 0; i < resolution_width; ++i)
    {
      for (unsigned j = 0; j < resolution_height; ++j)
      {
        double t = INFINITY;
        SceneObject *object = nullptr;

        Ray ray = cam.GetRay(i, j);

        for (std::vector<SceneObject *>::iterator object_iterator = objects.begin(); object_iterator < objects.end(); object_iterator++)
        {
          double new_t;
          if ((*object_iterator)->Intersect(ray, new_t))
          {
            if (new_t < t)
            {
              object = *object_iterator;
            }
          };
        }

        if (object)
        {
          image.SetPixel(i, j, red);
        }
        else
        {
          image.SetPixel(i, j, black);
        }
      }
    }

    image.Encode(filename);
  };

private:
  std::vector<SceneObject *> objects;
  Camera cam;
};

#endif