#ifndef INCLUDE_SCENE_H_
#define INCLUDE_SCENE_H_

#include <vector>
#include <iterator>
#include "sceneobject.h"
#include "camera.h"
#include "directionallight.h"

class Scene
{
public:
  Scene(Camera cam, lin_alg::Vector<3> background_colour, double ambient_intensity) : cam(cam), background(background_colour), ambient_intensity(ambient_intensity) {};

  ~Scene()
  {
    for (std::vector<SceneObject *>::iterator iter = objects.end() - 1; iter >= objects.begin(); iter--)
    {
      delete *iter;
    }
  };

  void AddObject(SceneObject *object);

  void AddLightSource(DirectionalLight *light);

  void Render(const char *filename, unsigned resolution_height, unsigned resolution_width);

private:
  std::vector<SceneObject *> objects;
  std::vector<DirectionalLight *> light_sources;
  Camera cam;
  lin_alg::Vector<3> background;
  double ambient_intensity;
};

#endif