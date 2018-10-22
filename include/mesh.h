#ifndef INCLUDE_MESH_H_
#define INCLUDE_MESH_H_
#include <vector>
#include <memory>
#include "vertex.h"
#include "face.h"
#include "matrix.h"
#include "vector.h"

// Implements Face-Vertex Mesh model
class Mesh
{
public:
  Mesh() : transformation_queue(1){};

  ~Mesh(){};

  void LoadObjectModel(const char *filename);
  void ExportObjectModel(const char *filename);
  void Translate(double x, double y, double z);
  void Rotate(lin_alg::Matrix<4>::Axis3D a, double angle);
  void AddRotation(lin_alg::Matrix<4>::Axis3D a, double angle);
  void AddTranslation(double x, double y, double z);

  void ExecuteTransformation()
  {
    UpdateVertices(transformation_queue);
    transformation_queue = lin_alg::Matrix<4>(1);
  };
  
  std::vector<Vertex> vertices;
  std::vector<Face> faces;

private:
  void UpdateVertices(lin_alg::Matrix<4> &transformation_mat);
  void WriteToFile(const char *filename, const int startingVertexIndex);
  lin_alg::Matrix<4> transformation_queue;
};

#endif