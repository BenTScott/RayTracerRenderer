#ifndef _INCLUDE_PHOTONPATHRAY_H_
#define _INCLUDE_PHOTONPATHRAY_H_

#include "ray.h"

class PhotonPathRay
{
  public:
    PhotonPathRay(){};
    PhotonPathRay(Ray ray, lin_alg::Vector<3> intensity) : ray(ray), intensity(intensity){};
    Ray ray;
    lin_alg::Vector<3> intensity;
};

#endif