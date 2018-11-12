#ifndef INCLUDE_SCENE_H_
#define INCLUDE_SCENE_H_

#include <vector>
#include <iterator>
#include "rgbimage.h"
#include "sceneobject.h"
#include "camera.h"
#include "light.h"
#include "lightingmodel.h"

class Scene
{
public:
  Scene(Camera cam, lin_alg::Vector<3> background_colour) : cam(cam), background(background_colour){};

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
  class Monitor
  {
  public:
    Monitor(double status_increment = 0.05) : status_increment(status_increment){};

    void Initialise(unsigned total_pixels)
    {
      this->total_pixels = total_pixels;
      current_pixel = 0;
      current_increment = status_increment;
    };

    void Increment()
    {
      current_pixel++;
      if ((double)current_pixel / (double)total_pixels >= current_increment)
      {
        std::cout << current_increment * 100 << "% done\n";
        current_increment += status_increment;
      }
    };

  private:
    double status_increment;
    unsigned total_pixels;
    unsigned current_pixel;
    double current_increment;
  };

  std::shared_ptr<RayIntersect> GetClosestIntersect(const Ray &ray) const;
  lin_alg::Vector<3> CalculateColourAtIntersect(const RayIntersect &intersect) const;
  lin_alg::Vector<3> GetColour(const Ray &ray) const;
  virtual RGBImage *GetImage(unsigned resolution_width, unsigned resolution_height);

  std::vector<SceneObject *> objects;
  std::vector<Light *> light_sources;

  Camera cam;
  lin_alg::Vector<3> background;
  LightingModel *lighting_model;

  Monitor *monitor = nullptr;

  Scene(){};
};

#endif