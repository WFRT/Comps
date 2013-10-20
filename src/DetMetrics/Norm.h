#ifndef DET_MERTIC_NORM_H
#define DET_MERTIC_NORM_H
#include "DetMetric.h"

class DetMetricNorm : public DetMetric {
   public:
      DetMetricNorm(const Options& iOptions, const Data& iData);

      float computeCore(const std::vector<std::pair<std::string, float> > & iData0,
                    const std::vector<std::pair<std::string, float> >& iData1,
                    const Parameters& iParameters,
                    const Data& iData) const;
      bool needsTraining() const {return false;};
   private:
      int mOrder;
};
#endif
