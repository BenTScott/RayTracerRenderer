#include <math.h>
#include <assert.h>
#include "rgbimage.h"
#include "lodepng.h"

void RGBImage::SetPixel(unsigned x, unsigned y, lin_alg::Vector<3> colour)
{
    assert(x >= 0 && y >= 0 && y < height && x < width);
    image[3 * width * y + 3 * x + 0] = (unsigned char) round(colour[0]*255.0);
    image[3 * width * y + 3 * x + 1] = (unsigned char) round(colour[1]*255.0);
    image[3 * width * y + 3 * x + 2] = (unsigned char) round(colour[2]*255.0);
};

void RGBImage::Encode(const char *filename)
{
    lodepng::encode(filename, image, width, height, LodePNGColorType::LCT_RGB);
};