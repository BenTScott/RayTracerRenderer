#ifndef _INCLUDE_PHOTON_H_
#define _INCLUDE_PHOTON_H_

#include "vector.h"

class Photon
{
  public:
    enum Type
    {
        Shadow,
        Direct,
        Indirect,
        Source
    };

    Photon(){};
    Photon(lin_alg::Vector<3> position, lin_alg::Vector<3> direction, lin_alg::Vector<3> intensity, Photon::Type type) : position(position), direction(direction), intensity(intensity), type(type){};

    lin_alg::Vector<3> position;
    lin_alg::Vector<3> direction;
    lin_alg::Vector<3> intensity;
    Photon::Type type;
};

#endif