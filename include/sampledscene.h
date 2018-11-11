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

    virtual void Render(const char *filename, unsigned resolution_width, unsigned resolution_height) override;

    void AddMonitoring();

  protected:
    virtual lin_alg::Vector<3> GetRayAverage(const std::vector<Ray> &rays);

    unsigned sample_rate;
    SamplingMethod method;
    bool monitor = false;
    SampledScene(){};    
};

#endif