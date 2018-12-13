#ifndef INCLUDE_MESH_H_
#define INCLUDE_MESH_H_

#include <vector>
#include <memory>
#include "vertex.h"
#include "face.h"
#include "matrix.h"
#include "vector.h"
#include "sceneobject.h"

// Implements Face-Vertex Mesh model
class Mesh : public virtual SceneObject
{
public:
  Mesh() : transformation_queue(1){};

  ~Mesh(){};

  void LoadObjectModel(const char *filename);
  [[deprecated]] void ExportObjectModel(const char *filename, bool overwrite = false);
  void Translate(double x, double y, double z);
  void Rotate(lin_alg::Axis3D a, double angle);
  void AddRotation(lin_alg::Axis3D a, double angle);
  void AddTranslation(double x, double y, double z);
  void AddScale(double scale);
  void MinMax(lin_alg::Vector<3> &min, lin_alg::Vector<3> &max);

  void SetColour(lin_alg::Vector<3> colour);

  void ExecuteTransformation()
  {
    UpdateVertices(transformation_queue);
    transformation_queue = lin_alg::Matrix<4>(1);
  };

  virtual std::shared_ptr<RayIntersect> Intersect(Ray ray) override;
  virtual std::vector<SurfacePoint> GetRandomPoints(unsigned samples) const override;

  void RecalculateNormals();
  
  std::vector<Vertex> vertices;
  std::vector<Face> faces;

private:
  void UpdateVertices(lin_alg::Matrix<4> &transformation_mat);
  void WriteToFile(const char *filename, const int startingVertexIndex, bool overwrite = false);
  lin_alg::Matrix<4> transformation_queue;
};

#endif