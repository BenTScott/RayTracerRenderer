#ifndef INCLUDE_SCENEOBJECT_H_
#define INCLUDE_SCENEOBJECT_H_

#include <memory>
#include "ray.h"

class SceneObject
{
  public:
    SceneObject(){};

    ~SceneObject(){};

    virtual bool Intersect(Ray ray, double &out_t) = 0;
};

#endif