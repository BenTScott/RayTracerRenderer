#include "camera.h"

void Camera::InitialiseResolution(unsigned width, unsigned height)
{
    resolution_height = height;
    resolution_width = width;
}

void Camera::InitialiseScreenSize(double width, double height)
{
    screen_height = height;
    screen_width = width;
}

Ray Camera::GetRay(int pixel_x, int pixel_y)
{
    double r = screen_width * ((pixel_x + 0.5) / resolution_width - 0.5);
    double b = screen_height * ((pixel_y + 0.5) / resolution_height - 0.5);
    lin_alg::Vector<3> pixel_location = camera_focalpoint + camera_forward.Scale(focallength) -
                                        camera_right.Scale(r) - camera_up.Scale(b);

    lin_alg::Vector<3> ray_dir = pixel_location - camera_focalpoint;

    Ray ray(camera_focalpoint, ray_dir);

    return ray;
}
