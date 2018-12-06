#ifndef _INCLUDE_SURFACEPOINT_H_
#define _INCLUDE_SURFACEPOINT_H_

#include "vector.h"

class SurfacePoint
{
  public:
    SurfacePoint() : point(), normal(){};
    SurfacePoint(lin_alg::Vector<3> point, lin_alg::Vector<3> normal) : point(point), normal(normal){};

    lin_alg::Vector<3> GetCorrectedPoint(double error = 0.001) const
    {
        return point + normal.Scale(error);
    };

    lin_alg::Vector<3> point;
    lin_alg::Vector<3> normal;
};

#endif