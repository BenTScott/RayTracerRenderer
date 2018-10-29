#ifndef INCLUDE_SCENEOBJECT_H_
#define INCLUDE_SCENEOBJECT_H_

#include <memory>
#include "ray.h"
#include "rayintersect.h"

class SceneObject
{
  public:
    SceneObject(){};

    virtual ~SceneObject(){};

    virtual std::shared_ptr<RayIntersect> Intersect(Ray ray) = 0;
};

#endif