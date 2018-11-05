#include <math.h>
#include <assert.h>
#include "rgbimage.h"
#include "lodepng.h"

// lin_alg::Vector<3> RGBImage::BoundColour(const lin_alg::Vector<3> &colour)
// {
//     lin_alg::Vector<3> bounded_colour;
//     for (int i = 0; i < 3; ++i)
//     {
//         double value = std::min(colour[i], 1.0);
//         value = std::max(value, 0.0);
//         bounded_colour[i] = value;
//     }
//     return bounded_colour;
// }

void RGBImage::SetPixel(unsigned x, unsigned y, lin_alg::Vector<3> colour)
{
    //lin_alg::Vector<3> bounded_colour = BoundColour(colour);
    assert(y < height && x < width);
    image[3 * width * y + 3 * x + 0] = (unsigned char)round(colour[0] * 255.0);
    image[3 * width * y + 3 * x + 1] = (unsigned char)round(colour[1] * 255.0);
    image[3 * width * y + 3 * x + 2] = (unsigned char)round(colour[2] * 255.0);
};

void RGBImage::Encode(const char *filename)
{
    lodepng::encode(filename, image, width, height, LodePNGColorType::LCT_RGB);
};