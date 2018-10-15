#include <list>
#include "vertex.h"

#ifndef INCLUDE_FACE_H_
#define INCLUDE_FACE_H_

class Face
{
public:
  Face(){};
  Face(Vertex* v1, Vertex* v2, Vertex* v3) : vertices{v1, v2, v3} {};

  ~Face(){};

  Vertex* vertices[3];
};

#endif