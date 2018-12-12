#ifndef INCLUDE_RGBIMAGE_H_
#define INCLUDE_RGBIMAGE_H_

#include "vector.h"
#include "matrix.h"
#include <vector>

class RGBImage
{
  public:
    friend class ThreadSafeImage;

    RGBImage() : width(10), height(10)
    {
        image.resize(width * height);
    };
    RGBImage(unsigned width, unsigned height) : width(width), height(height)
    {
        image.resize(width * height);
    };
    RGBImage(RGBImage &obj)
    {
        width = obj.width;
        height = obj.height;
        image = obj.image;
    };
    RGBImage(const char *filename, unsigned width, unsigned height);

    virtual ~RGBImage(){};

    unsigned Width()
    {
        return width;
    };

    unsigned Height()
    {
        return height;
    };

    virtual void SetPixel(unsigned x, unsigned y, lin_alg::Vector<3> colour);
    lin_alg::Vector<3> GetPixel(unsigned x, unsigned y);
    void ApplyKernel(lin_alg::Matrix<3> kernel, double scale = 1);
    void ApplyKernel(lin_alg::Matrix<3> kernel, double scale, unsigned pass_count);
    void Encode(const char *filename);

  protected:
    unsigned width;
    unsigned height;
    std::vector<lin_alg::Vector<3>> image;
};

#endif