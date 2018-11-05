#ifndef INCLUDE_SCENE_H_
#define INCLUDE_SCENE_H_

#include <vector>
#include <iterator>
#include "sceneobject.h"
#include "camera.h"
#include "Light.h"

class Scene
{
public:
  Scene(Camera cam, lin_alg::Vector<3> background_colour, double ambient_intensity) : cam(cam), background(background_colour), ambient_intensity(ambient_intensity){};

  ~Scene()
  {
    for (std::vector<SceneObject *>::iterator iter = objects.end() - 1; iter >= objects.begin(); iter--)
    {
      delete *iter;
    }
    for (std::vector<Light *>::iterator iter = light_sources.end() - 1; iter >= light_sources.begin(); iter--)
    {
      delete *iter;
    }
  };

  void AddObject(SceneObject *object);

  void AddLightSource(Light *light);

  void Render(const char *filename, unsigned resolution_width, unsigned resolution_height);

  void Render(const char *filename, unsigned resolution_width, unsigned resolution_height, unsigned sample_rate);

private:
  bool InShadow(Ray &lightray);
  lin_alg::Vector<3> BoundColour(const lin_alg::Vector<3> &colour);
  std::shared_ptr<RayIntersect> GetClosestIntersect(Ray &ray);
  lin_alg::Vector<3> CalculateColourAtIntersect(Ray &ray, RayIntersect &intersect);
  lin_alg::Vector<3> GetColour(Ray &ray);

  std::vector<SceneObject *> objects;
  std::vector<Light *> light_sources;
  Camera cam;
  lin_alg::Vector<3> background;
  double ambient_intensity;
};

#endif