#ifndef INCLUDE_SCENEOBJECT_H_
#define INCLUDE_SCENEOBJECT_H_

#include <memory>
#include "ray.h"
#include "surfacepoint.h"
#include "rayintersect.h"
#include "material.h"

class SceneObject
{
  public:
    SceneObject(){};
    SceneObject(Material material) : material(material) {};

    virtual ~SceneObject(){};

    virtual std::shared_ptr<RayIntersect> Intersect(Ray ray) = 0;
    virtual std::vector<SurfacePoint> GetRandomPoints(unsigned samples) const = 0;

    Material material;
};

#endif