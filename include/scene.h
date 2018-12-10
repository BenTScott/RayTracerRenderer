#ifndef INCLUDE_SCENE_H_
#define INCLUDE_SCENE_H_

#include <vector>
#include <iterator>
#include "rgbimage.h"
#include "sceneobject.h"
#include "camera.h"
#include "light.h"
#include "lightingmodel.h"
#include "taskmonitor.h"

class Scene
{
public:
  Scene(Camera cam, lin_alg::Vector<3> background_colour) : cam(cam), background(background_colour){};

  virtual ~Scene()
  {
    for (auto obj_ptr : objects)
    {
      delete obj_ptr;
    }
    for (auto light_ptr : light_sources)
    {
      delete light_ptr;
    }

    if (monitor)
    {
      delete monitor;
    }
  };

  void Render(const char *filename, unsigned resolution_width, unsigned resolution_height);

  void AddObject(SceneObject *object);

  void AddLightSource(Light *light);

  void AddMonitoring();

  void SetLightingModel(LightingModel *lighting_model);

  bool InShadow(Ray &lightray) const;
  
protected:
  virtual lin_alg::Vector<3> CalculateColourAtIntersect(const RayIntersect &intersect, unsigned depth = 0) const;
  std::shared_ptr<RayIntersect> GetRayIntersect(Ray ray);
  lin_alg::Vector<3> GetColour(const Ray &ray, unsigned depth = 0) const;
  virtual RGBImage *GetImage(unsigned resolution_width, unsigned resolution_height);

  std::vector<SceneObject *> objects;
  std::vector<Light *> light_sources;

  Camera cam;
  lin_alg::Vector<3> background;
  LightingModel *lighting_model;

  TaskMonitor *monitor = nullptr;

  unsigned max_reflection_depth = 10;

  Scene(){};
};

#endif