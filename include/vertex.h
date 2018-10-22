#include <list>
#include <iterator>
#include <algorithm>
#include "vector.h"

class Face;

#ifndef INCLUDE_VERTEX_H_
#define INCLUDE_VERTEX_H_

class Vertex
{
public:
  Vertex(){};
  Vertex(double x, double y, double z) {
    pos[0] = x;
    pos[1] = y;
    pos[2] = z;
    pos[3] = 1;
  };

  ~Vertex(){};

  void Update_Position(lin_alg::Vector<4> new_pos)
  {
    for (int i = 0; i < 3; i++)
    {
      pos[i] = new_pos[i];
    }
  }

  lin_alg::Vector<4> pos;
  std::list<Face *> faces;
};

#endif