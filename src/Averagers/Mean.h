#ifndef AVERAGER_MEAN_H
#define AVERAGER_MEAN_H
#include "../Global.h"
#include "../Options.h"
#include "Averager.h"

//! Computes the mean of the distribution by sampling several quantiles
class AveragerMean : public Averager {
   public:
      AveragerMean(const Options& iOptions, const Data& iData);
      float average(const Distribution& iDist, const Parameters& iParameters) const;
      void getDefaultParameters(Parameters& iParameters) const {};
      bool needsTraining() const {return false;};
   private:
      int mNumPoints;
};
#endif
