#ifndef _INCLUDE_PHOTON_H_
#define _INCLUDE_PHOTON_H_

#include "vector.h"

class Photon
{
  public:
    enum Type
    {
        // No final gathering implemented so direct photons are not stored.
        Shadow,
        Indirect
    };

    Photon(){};
    Photon(lin_alg::Vector<3> position, lin_alg::Vector<3> direction, lin_alg::Vector<3> intensity, Photon::Type type) : position(position), direction(direction), intensity(intensity), type(type)
    {
        this->direction.Normalise();
    };

    lin_alg::Vector<3> position;
    lin_alg::Vector<3> direction;
    lin_alg::Vector<3> intensity;
    lin_alg::Vector<3> normal;
    Photon::Type type;
};

#endif