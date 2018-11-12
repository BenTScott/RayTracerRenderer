#ifndef INCLUDE_SAMPLEDSCENE_H_
#define INCLUDE_SAMPLEDSCENE_H_

#include "scene.h"

class SampledScene : public Scene
{
  public:
    enum SamplingMethod
    {
        Random,
        Jitter
    };

    SampledScene(Camera cam, lin_alg::Vector<3> background_colour, unsigned sample_rate, SamplingMethod method)
        : Scene(cam, background_colour), sample_rate(sample_rate), method(method){};

    ~SampledScene(){};

    void AddMonitoring();

  protected:
    class Monitor;

    virtual lin_alg::Vector<3> GetRayAverage(const std::vector<Ray> &rays);
    virtual RGBImage *GetImage(unsigned resolution_width, unsigned resolution_height) override;
    Monitor *monitor = nullptr;

    unsigned sample_rate;
    SamplingMethod method;
    SampledScene(){};
};

class SampledScene::Monitor
{
  public:
    Monitor(double status_increment = 0.05) : status_increment(status_increment){};

    void Initialise(unsigned total_pixels)
    {
        this->total_pixels = total_pixels;
        current_pixel = 0;
        current_increment = status_increment;
    };

    void Increment()
    {
        current_pixel++;
        if ((double)current_pixel / (double)total_pixels >= current_increment)
        {
            std::cout << current_increment * 100 << "% done\n";
            current_increment += status_increment;
        }
    };

  private:
    double status_increment;
    unsigned total_pixels;
    unsigned current_pixel;
    double current_increment;
};

#endif