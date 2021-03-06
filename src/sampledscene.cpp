#include "sampledscene.h"
#include "rgbimage.h"
#include <iostream>

RGBImage *SampledScene::GetImage(unsigned resolution_width, unsigned resolution_height)
{
    RGBImage *image = new RGBImage(resolution_width, resolution_height);

    unsigned total_pixels = resolution_height * resolution_width;
    monitor->Initialise(total_pixels);

    for (unsigned i = 0; i < resolution_width; ++i)
    {
        for (unsigned j = 0; j < resolution_height; ++j)
        {
            if (monitor)
            {
                monitor->Increment();
            }

            std::vector<Ray> rays = method == SampledScene::Random ? cam.GetRandomRaySamples(i, j, sample_rate)
                                                                   : cam.GetJitterRaySamples(i, j, sample_rate);

            image->SetPixel(i, j, GetRayAverage(rays));
        }
    }

    return image;
}

lin_alg::Vector<3> SampledScene::GetRayAverage(const std::vector<Ray> &rays)
{
    lin_alg::Vector<3> colour;

    for (const Ray &ray : rays)
    {
        colour += GetColour(ray);
    }

    // Get average colour
    return colour.Scale(1.0 / rays.size());
}