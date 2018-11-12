#include "adaptivesampledscene.h"
#include "matrix.h"

RGBImage *AdaptiveSampledScene::GetSampleRates(unsigned resolution_width, unsigned resolution_height)
{
    // Do a simple first pass
    RGBImage *image = Scene::GetImage(resolution_width, resolution_height);
    RGBImage *copy = new RGBImage(*image);

    image->Encode(".\\out\\test2.png");
    
    lin_alg::Matrix<3> kernel1;
    lin_alg::Matrix<3> kernel2;

    for (int i = 0; i < 3; ++i)
    {
        for (int j = 0; j < 3; ++j)
        {
            if (i == 1 && j == 1)
            {
                kernel1(i, j) = 8;
            }
            else
            {
                kernel1(i, j) = -1;
            }
            kernel2(i, j) = 1;
        }
    }

    copy->ApplyKernel(kernel1);
    copy->ApplyKernel(kernel2, 1.0 / 5.0);
    copy->ApplyKernel(kernel2, 1.0 / 5.0);
    copy->ApplyKernel(kernel2, 1.0 / 5.0);

    //copy->Encode(".\\out\\test.png");

    // Pre-allocate
    sample_rates.resize(resolution_width);
    for (unsigned i = 0; i < resolution_width; ++i)
    {
        sample_rates[i].resize(resolution_height);
    }

    for (unsigned i = 0; i < resolution_width; i++)
    {
        for (unsigned j = 0; j < resolution_height; j++)
        {
            sample_rates[i][j] = std::round(image->GetPixel(i, j).Max() * (double)(sample_rate - 1)) + 1;
        }
    }

    delete copy;

    return image;
};

RGBImage *AdaptiveSampledScene::GetImage(unsigned resolution_width, unsigned resolution_height)
{
    std::cout << "Performing first pass\n";

    RGBImage *image = GetSampleRates(resolution_width, resolution_height);

    std::cout << "Starting render\n";

    cam.InitialiseResolution(resolution_width, resolution_height);
    //RGBImage *image = new RGBImage(resolution_width, resolution_height);
    
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

            if (sample_rates[i][j] > 1)
            {
                unsigned pixel_sample_rate = sample_rates[i][j];

                std::vector<Ray> rays = method == SampledScene::Random ? cam.GetRandomRaySamples(i, j, pixel_sample_rate)
                                                                       : cam.GetJitterRaySamples(i, j, pixel_sample_rate);

                image->SetPixel(i, j, GetRayAverage(rays));
            }
        }
    }

    return image;
}