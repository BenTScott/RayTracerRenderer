#ifndef INCLUDE_2DSHAPES_H_
#define INCLUDE_2DSHAPES_H_

#include "vector.h"
#include "sceneobject.h"
#include "mesh.h"

class Rectangle : public Mesh
{
  public:
    // Order matters - anticlockwise
    Rectangle(lin_alg::Vector<3> p1, lin_alg::Vector<3> p2, lin_alg::Vector<3> p3, lin_alg::Vector<3> colour)
    {
        Initialise(p1, p2, p3, colour);
    };
    Rectangle()
    {
        Initialise({1, 0, -1}, {-1, 0, -1}, {-1, 0, 1}, {1, 1, 1});
    };

    virtual ~Rectangle(){};

    virtual std::shared_ptr<RayIntersect> Intersect(Ray ray) override;

  private:
    void Initialise(lin_alg::Vector<3> p1, lin_alg::Vector<3> p2, lin_alg::Vector<3> p3, lin_alg::Vector<3> colour);
};

class Circle : SceneObject
{
  public:
    Circle(lin_alg::Vector<3> centre, double radius, lin_alg::Vector<3> normal, lin_alg::Vector<3> colour) : SceneObject(colour), centre(centre), radius(radius), normal(normal){};
    Circle() : SceneObject(Material({1, 1, 1})), centre({0, 0, 0}), radius(1.0), normal({0, 1, 0}){};
   // Circle(Circle &obj) : centre(obj.centre), radius(obj.radius), normal(obj.normal), colour(obj.colour) {};
    virtual ~Circle(){};

    virtual std::shared_ptr<RayIntersect> Intersect(Ray ray) override;
    virtual std::vector<SurfacePoint> GetRandomPoints(unsigned samples) const override;

  protected:
    lin_alg::Vector<3> centre;
    double radius;
    lin_alg::Vector<3> normal;
    //lin_alg::Vector<3> colour;
};

#endif