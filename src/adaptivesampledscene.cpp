#include "adaptivesampledscene.h"
#include "matrix.h"

RGBImage *AdaptiveSampledScene::GetSampleRates(unsigned resolution_width, unsigned resolution_height)
{
    // Do a simple first pass
    RGBImage *image = Scene::GetImage(resolution_width, resolution_height);
    RGBImage *copy = new RGBImage(*image);

    ApplyKernelTransformations(copy);

    BuildTaskList(copy);

    delete copy;

    return image;
};

void AdaptiveSampledScene::BuildTaskList(RGBImage *first_pass)
{
    // Reserve max size
    tasks.clear();
    tasks.reserve(first_pass->Width() * first_pass->Height());
    
    for (unsigned i = 0; i < first_pass->Width(); i++)
    {
        for (unsigned j = 0; j < first_pass->Height(); j++)
        {
            double max = first_pass->GetPixel(i, j).Max();
            unsigned pixel_sample_rate = std::round(max * (sample_rate - 1)) + 1;
            PixelTask task;
            task.pixel_x = i;
            task.pixel_y = j;
            task.sample_rate = pixel_sample_rate;
            tasks.push_back(task);
        }
    }
};

void AdaptiveSampledScene::ApplyKernelTransformations(RGBImage *first_pass)
{
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
    first_pass->ApplyKernel(kernel1);

    // Blur
    first_pass->ApplyKernel(kernel2, 1.0 / 5.0, 7);
}

RGBImage *AdaptiveSampledScene::GetImage(unsigned resolution_width, unsigned resolution_height)
{
    std::cout << "Performing first pass\n";

    RGBImage *image = GetSampleRates(resolution_width, resolution_height);

    image->Encode("first_pass.png");

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