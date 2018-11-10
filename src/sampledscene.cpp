#include "sampledscene.h"
#include "rgbimage.h"

void SampledScene::Render(const char *filename, unsigned resolution_width, unsigned resolution_height)
{
    cam.InitialiseResolution(resolution_width, resolution_height);
    RGBImage image(resolution_width, resolution_height);
    sample_rate = pow(ceil(sqrt(sample_rate)), 2);
    for (unsigned i = 0; i < resolution_width; ++i)
    {
        for (unsigned j = 0; j < resolution_height; ++j)
        {
            std::vector<Ray> rays = method == SampledScene::Random ? cam.GetRandomRaySamples(i, j, sample_rate)
                                                                   : cam.GetJitterRaySamples(i, j, sample_rate);

            lin_alg::Vector<3> colour;

            for (std::vector<Ray>::iterator ray_iterator = rays.begin(); ray_iterator < rays.end(); ray_iterator++)
            {
                colour += GetColour(*ray_iterator);
            }

            // Get average colour
            lin_alg::Vector<3> av_col = colour.Scale(1.0 / sample_rate);

            image.SetPixel(i, j, av_col);
        }
    }

    image.Encode(filename);
};