#include "sampledscene.h"
#include "rgbimage.h"
#include <iostream>
#include <ctime>

void SampledScene::AddMonitoring()
{
    this->monitor = true;
};

void SampledScene::Render(const char *filename, unsigned resolution_width, unsigned resolution_height)
{
    cam.InitialiseResolution(resolution_width, resolution_height);
    RGBImage image(resolution_width, resolution_height);
    sample_rate = pow(ceil(sqrt(sample_rate)), 2);
    double status_increment = 0.05;
    double pixels_rendered = 0;
    double total_pixels = resolution_height*resolution_width;
    for (unsigned i = 0; i < resolution_width; ++i)
    {
        for (unsigned j = 0; j < resolution_height; ++j)
        {
            if (monitor)
            {
                pixels_rendered++;
                if (pixels_rendered/total_pixels >= status_increment)
                {
                    std::cout << status_increment * 100 << "% done\n";
                    status_increment += 0.5;
                }
            }

            std::vector<Ray> rays = method == SampledScene::Random ? cam.GetRandomRaySamples(i, j, sample_rate)
                                                                   : cam.GetJitterRaySamples(i, j, sample_rate);

            image.SetPixel(i, j, GetRayAverage(rays));
        }
    }

    image.Encode(filename);
};

lin_alg::Vector<3> SampledScene::GetRayAverage(const std::vector<Ray> &rays)
{
    lin_alg::Vector<3> colour;

    for (std::vector<Ray>::const_iterator ray_iterator = rays.begin(); ray_iterator < rays.end(); ray_iterator++)
    {
        colour += GetColour(*ray_iterator);
    }

    // Get average colour
    return colour.Scale(1.0 / sample_rate);
}