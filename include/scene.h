#ifndef INCLUDE_SCENE_H_
#define INCLUDE_SCENE_H_

#include <vector>
#include <iterator>
#include "sceneobject.h"
#include "camera.h"

class Scene
{
public:
  Scene(Camera cam, lin_alg::Vector<3> background_colour) : cam(cam), background(background_colour){};

  ~Scene()
  {
    for (std::vector<SceneObject *>::iterator iter = objects.end() - 1; iter >= objects.begin(); iter--)
    {
      delete *iter;
    }
  };

  void AddObject(SceneObject *object);

  void Render(const char *filename, unsigned resolution_height, unsigned resolution_width);

private:
  std::vector<SceneObject *> objects;
  Camera cam;
  lin_alg::Vector<3> background;
};

#endif