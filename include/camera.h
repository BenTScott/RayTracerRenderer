#ifndef INCLUDE_CAMERA_H_
#define INCLUDE_CAMERA_H_

#include "vector.h"
#include "ray.h"

class Camera
{
public:
  Camera();
  Camera(lin_alg::Vector<3> camera_up, lin_alg::Vector<3> camera_forward, lin_alg::Vector<3> camera_focalpoint, double focallength)
      : camera_up(camera_up), camera_forward(camera_forward), camera_focalpoint(camera_focalpoint), focallength(focallength)
  {
    Camera::camera_forward.Normalise();
    Camera::camera_up.Normalise();
    camera_right = lin_alg::CrossProduct(camera_up, camera_forward);
    screen_width = 1;
    screen_height = 1;
    resolution_width = 100;
    resolution_height = 100;
  };

  void InitialiseScreenSize(double width, double height);

  void InitialiseResolution(unsigned width, unsigned height);

  /*
   Returns a ray through the centre of the specified pixel
   (0,0) is the top left of the screen
  */
  Ray GetRay(int pixel_x, int pixel_y);

  lin_alg::Vector<3> camera_up;
  lin_alg::Vector<3> camera_forward;
  lin_alg::Vector<3> camera_focalpoint;
  double focallength;

  double screen_width;
  double screen_height;

  unsigned resolution_width;
  unsigned resolution_height;

  // Calculated
  lin_alg::Vector<3> camera_right;
};

#endif