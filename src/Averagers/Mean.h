#ifndef AVERAGER_MEAN_H
#define AVERAGER_MEAN_H
#include "../Global.h"
#include "../Options.h"

class AveragerMean : public Averager {
   public:
      AveragerMean(const Options& iOptions, const Data& iData);
      float average(const Ensemble& iValues, const Parameters& iParameters) const;
      void getDefaultParameters(Parameters& iParameters) const {}
      bool needsTraining() const {return false;};
};
#endif
