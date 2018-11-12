#ifndef INCLUDE_ADAPTIVESAMPLEDSCENE_H_
#define INCLUDE_ADAPTIVESAMPLEDSCENE_H_

#include "sampledscene.h"

class AdaptiveSampledScene : public SampledScene
{
  public:
    virtual ~AdaptiveSampledScene(){};

    AdaptiveSampledScene(Camera cam, lin_alg::Vector<3> background_colour, unsigned max_sample_rate, SamplingMethod method) :
      SampledScene(cam, background_colour, max_sample_rate, method){};
    
  protected:
    std::vector<std::vector<unsigned>> sample_rates;
    virtual RGBImage *GetImage(unsigned resolution_width, unsigned resolution_height) override;
    RGBImage *GetSampleRates(unsigned resolution_width, unsigned resolution_height);
    AdaptiveSampledScene(){};
};

#endif