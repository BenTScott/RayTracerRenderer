#include <list>
#include <iterator>
#include <algorithm>

class Face;

#ifndef INCLUDE_VERTEX_H_
#define INCLUDE_VERTEX_H_

class Vertex
{
public:
  Vertex(){};
  Vertex(double x, double y, double z) : pos{x, y, z, 1} {};

  ~Vertex(){};

  void Update_Position(double new_pos[4])
  {
    for (int i = 0; i < 3; i++)
    {
      pos[i] = new_pos[i];
    }
  }

  double pos[4];
  std::list<Face *> faces;
};

#endif