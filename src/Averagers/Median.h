#ifndef AVERAGER_MEDIAN_H
#define AVERAGER_MEDIAN_H
#include "../Global.h"
#include "../Options.h"

class AveragerMedian : public Averager {
   public:
      AveragerMedian(const Options& iOptions, const Data& iData);
      float average(const Ensemble& iValues, const Parameters& iParameters) const;
      void getDefaultParameters(Parameters& iParameters) const {};
      bool needsTraining() const {return false;};
};
#endif
