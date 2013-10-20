#ifndef AVERAGER_REGRESSION_H
#define AVERAGER_REGRESSION_H
#include "../Global.h"
#include "../Options.h"

//! Use ensemble members as variates in a multiple linear regression.
class AveragerRegression : public Averager {
   public:
      AveragerRegression(const Options& iOptions, const Data& iData);
      float average(const Ensemble& iValues, const Parameters& iParameters) const;
      void getDefaultParameters(Parameters& iParameters) const {}
};
#endif
