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

    SampledScene(Camera cam, lin_alg::Vector<3> background_colour, LightingModel *lighting_model, unsigned sample_rate, SamplingMethod method)
        : Scene(cam, background_colour, lighting_model), sample_rate(sample_rate), method(method){};

    ~SampledScene(){};

    virtual void Render(const char *filename, unsigned resolution_width, unsigned resolution_height) override;

  protected:
    unsigned sample_rate;
    SamplingMethod method;
    SampledScene(){};    
};

#endif