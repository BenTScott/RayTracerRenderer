#ifndef INCLUDE_SCENE_H_
#define INCLUDE_SCENE_H_

#include <vector>
#include <iterator>
#include "sceneobject.h"
#include "camera.h"
#include "light.h"
#include "lightingmodel.h"

class Scene
{
public:
  Scene(Camera cam, lin_alg::Vector<3> background_colour, LightingModel *lighting_model) : cam(cam), background(background_colour), lighting_model(lighting_model){};

  virtual ~Scene()
  {
    for (std::vector<SceneObject *>::reverse_iterator iter = objects.rbegin(); iter >= objects.rend(); ++iter)
    {
      delete *iter;
    }
    for (std::vector<Light *>::reverse_iterator iter = light_sources.rbegin(); iter >= light_sources.rend(); ++iter)
    {
      delete *iter;
    }
  };

  virtual void Render(const char *filename, unsigned resolution_width, unsigned resolution_height);

  void AddObject(SceneObject *object);

  void AddLightSource(Light *light);

  bool InShadow(Ray &lightray) const;

protected:
  std::shared_ptr<RayIntersect> GetClosestIntersect(Ray &ray);
  lin_alg::Vector<3> CalculateColourAtIntersect(RayIntersect &intersect);
  lin_alg::Vector<3> GetColour(Ray &ray);

  std::vector<SceneObject *> objects;
  std::vector<Light *> light_sources;

  Camera cam;
  lin_alg::Vector<3> background;
  LightingModel *lighting_model;

  Scene(){};
};

#endif