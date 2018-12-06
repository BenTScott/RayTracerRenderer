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

    // TODO: Change to return RayIntersect and then pass around by ref
    virtual std::shared_ptr<RayIntersect> Intersect(Ray ray) = 0;
    virtual std::vector<SurfacePoint> GetRandomPoints(unsigned samples) const = 0;

    Material material;
};

#endif