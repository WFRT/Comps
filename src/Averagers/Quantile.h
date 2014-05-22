#ifndef AVERAGER_QUANTILE_H
#define AVERAGER_QUANTILE_H
#include "../Global.h"
#include "../Options.h"

class AveragerQuantile : public Averager {
   public:
      AveragerQuantile(const Options& iOptions, const Data& iData);
      float average(const Distribution& iDist, const Parameters& iParameters) const;
      void getDefaultParameters(Parameters& iParameters) const {};
      bool needsTraining() const {return false;};
   private:
      float mQuantile;
};
#endif
