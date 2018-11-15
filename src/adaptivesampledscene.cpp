#include "adaptivesampledscene.h"
#include "matrix.h"

RGBImage *AdaptiveSampledScene::GetSampleRates(unsigned resolution_width, unsigned resolution_height)
{
    // Do a simple first pass
    RGBImage *image = Scene::GetImage(resolution_width, resolution_height);
    RGBImage *copy = new RGBImage(*image);

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

    // Apply edge detection kernel
    copy->ApplyKernel(kernel1);

    // Blur
    copy->ApplyKernel(kernel2, 1.0 / 5.0, 7);

    // Reserve max size
    tasks.clear();
    tasks.reserve(resolution_width * resolution_height);

    for (unsigned i = 0; i < resolution_width; i++)
    {
        for (unsigned j = 0; j < resolution_height; j++)
        {
            double max = copy->GetPixel(i, j).Max();
            unsigned pixel_sample_rate = std::round(max * (sample_rate - 1)) + 1;
            PixelTask task;
            task.pixel_x = i;
            task.pixel_y = j;
            task.sample_rate = pixel_sample_rate;
            tasks.push_back(task);
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

    monitor->Initialise(tasks.size());

    for (const PixelTask &t : tasks)
    {
        if (monitor)
        {
            monitor->Increment();
        }

        std::vector<Ray> rays = method == SampledScene::Random ? cam.GetRandomRaySamples(t.pixel_x, t.pixel_y, t.sample_rate)
                                                               : cam.GetJitterRaySamples(t.pixel_x, t.pixel_y, t.sample_rate);

        image->SetPixel(t.pixel_x, t.pixel_y, GetRayAverage(rays));
    }

    return image;
}