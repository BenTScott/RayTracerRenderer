#ifndef INCLUDE_THREADSAFEIMAGE_H_
#define INCLUDE_THREADSAFEIMAGE_H_

#include "rgbimage.h"
#include <mutex>

class ThreadSafeImage : public RGBImage
{
public:
    ThreadSafeImage(unsigned width, unsigned height) : RGBImage(width, height) {}

    ThreadSafeImage(RGBImage &img)
    {
        this->image = img.image;
        this->height = img.height;
        this->width = img.width;
    }

    ~ThreadSafeImage(){};

    virtual void SetPixel(unsigned x, unsigned y, lin_alg::Vector<3> colour) override
    {
        std::lock_guard<std::mutex> lock(m);
        RGBImage::SetPixel(x, y, colour);
    };

protected:
    std::mutex m;
};

#endif