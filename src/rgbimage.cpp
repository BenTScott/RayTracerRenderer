#include <math.h>
#include <assert.h>
#include "rgbimage.h"
#include "lodepng.h"

RGBImage::RGBImage(const char *filename, unsigned width, unsigned height) : width(width), height(height)
{
    std::vector<unsigned char> char_image;

    unsigned error = lodepng::decode(char_image, width, height, filename);

    //if there's an error, display it
    if (error)
    {
        std::cout << "decoder error " << error << ": " << lodepng_error_text(error) << std::endl;
        return;
    }

    image.resize(width * height);

    for (unsigned i = 0; i < width; ++i)
    {
        for (unsigned j = 0; j < height; ++j)
        {
            lin_alg::Vector<3> col = 
            {
                (double)char_image[4 * width * j + 4 * i + 0],
                (double)char_image[4 * width * j + 4 * i + 1],
                (double)char_image[4 * width * j + 4 * i + 2]
            };
            SetPixel(i, j, col.Scale(1.0/255.0));
        }
    }
};

void RGBImage::SetPixel(unsigned x, unsigned y, lin_alg::Vector<3> colour)
{
    assert(y < height && x < width);
    image[width * y + x] = colour;
};

void RGBImage::ApplyKernel(lin_alg::Matrix<3> kernel, double scale)
{
    std::vector<lin_alg::Vector<3>> new_image;
    new_image.resize(width * height);

    for (unsigned i = 0; i < width; ++i)
    {
        for (unsigned j = 0; j < height; ++j)
        {
            if (i != 0 && j != 0 && i != width - 1 && j != height - 1)
            {
                lin_alg::Vector<3> block[3][3] = {
                    {GetPixel(i - 1, j + 1), GetPixel(i - 1, j), GetPixel(i - 1, j - 1)},
                    {GetPixel(i, j + 1), GetPixel(i, j), GetPixel(i, j + 1)},
                    {GetPixel(i + 1, j + 1), GetPixel(i + 1, j), GetPixel(i + 1, j - 1)}};

                new_image[width * j + i] = kernel.Apply(block).Scale(scale).Bound();
            }
        }
    }

    image = new_image;
}

void RGBImage::ApplyKernel(lin_alg::Matrix<3> kernel, double scale, unsigned pass_count)
{
    for (unsigned i = 0; i < pass_count; ++i)
    {
        ApplyKernel(kernel, scale);
    }
}

lin_alg::Vector<3> RGBImage::GetPixel(unsigned x, unsigned y)
{
    return image[width * y + x];
}

void RGBImage::Encode(const char *filename)
{
    std::vector<unsigned char> char_image;
    char_image.resize(width * height * 3);
    for (unsigned i = 0; i < width; ++i)
    {
        for (unsigned j = 0; j < height; ++j)
        {
            lin_alg::Vector<3> pixel = GetPixel(i, j);
            char_image[3 * width * j + 3 * i + 0] = (unsigned char)round(pixel[0] * 255.0);
            char_image[3 * width * j + 3 * i + 1] = (unsigned char)round(pixel[1] * 255.0);
            char_image[3 * width * j + 3 * i + 2] = (unsigned char)round(pixel[2] * 255.0);
        }
    }

    lodepng::encode(filename, char_image, width, height, LodePNGColorType::LCT_RGB);
};