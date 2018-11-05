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

Ray Camera::GetRay(unsigned pixel_x, unsigned pixel_y)
{
    return GetRay(pixel_x, pixel_y, 0.5, 0.5);
}

std::vector<Ray> Camera::GetRandomRaySamples(unsigned pixel_x, unsigned pixel_y, unsigned sampling_rate)
{
    std::vector<Ray> rays;
    for (unsigned i = 0; i < sampling_rate; ++i)
    {
        double distance_x = (double) rand() / (double) (RAND_MAX + 1);
        double distance_y = (double) rand() / (double) (RAND_MAX + 1);
        rays.push_back(GetRay(pixel_x, pixel_y, distance_x, distance_y));
    }
    return rays;
}

// Gets a ray through a pixel where distance_x and distance_y are distances 
Ray Camera::GetRay(unsigned pixel_x, unsigned pixel_y, double distance_x, double distance_y)
{
    double r = screen_width * ((pixel_x + distance_x) / resolution_width - 0.5);
    double b = screen_height * ((pixel_y + distance_y) / resolution_height - 0.5);
    lin_alg::Vector<3> pixel_location = camera_focalpoint + camera_forward.Scale(focallength) -
                                        camera_right.Scale(r) - camera_up.Scale(b);

    lin_alg::Vector<3> ray_dir = pixel_location - camera_focalpoint;

    Ray ray(camera_focalpoint, ray_dir);

    return ray;
}