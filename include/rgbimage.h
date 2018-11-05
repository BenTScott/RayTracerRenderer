#ifndef INCLUDE_RGBIMAGE_H_
#define INCLUDE_RGBIMAGE_H_

#include "vector.h"
#include <vector>

class RGBImage
{
  public:
    RGBImage() : width(10), height(10)
    {
        image.resize(width * height * 3);
    };
    RGBImage(unsigned width, unsigned height) : width(width), height(height)
    {
        image.resize(width * height * 3);
    };
    RGBImage(RGBImage &obj)
    {
        width = obj.width;
        height = obj.height;
        image = obj.image;
    };

    ~RGBImage(){};

    void SetPixel(unsigned x, unsigned y, lin_alg::Vector<3> colour);

    void Encode(const char *filename);

  private:
    unsigned width;
    unsigned height;
    std::vector<unsigned char> image;
};

#endif