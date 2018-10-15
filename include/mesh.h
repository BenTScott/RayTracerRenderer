#include <vector>
#include <memory>
#include "vertex.h"
#include "face.h"
#include "matrix.h"

using namespace std;

#ifndef INCLUDE_MESH_H_
#define INCLUDE_MESH_H_

// Implements Face-Vertex Mesh model
class Mesh
{
public:
  Mesh() : transformation_queue(1) {};

  ~Mesh(){};

  void LoadObjectModel(const char *filename);
  void ExportObjectModel(const char *filename);
  void Translate(double x, double y, double z);
  void Rotate(Matrix<4>::Axis3D a, double angle);
  void AddRotation(Matrix<4>::Axis3D a, double angle);
  void AddTranslation(double x, double y, double z);

  void ExecuteTransformation()
  {
    UpdateVertices(transformation_queue);
    transformation_queue = Matrix<4>(1);
  };

  vector<Vertex> vertices;
  vector<Face> faces;

private:
  void UpdateVertices(Matrix<4> &transformation_mat);
  void WriteToFile(const char *filename, const int startingVertexIndex);
  Matrix<4> transformation_queue;
};

#endif