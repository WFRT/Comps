#ifndef CONTINUOUS_UPSTREAM_H
#define CONTINUOUS_UPSTREAM_H
#include "../Global.h"
#include "../Options.h"
#include "../Scheme.h"

class ContinuousUpstream : public Continuous {
   public:
      ContinuousUpstream(const Options& rOptions, const Data& iData);
      //~ContinuousUpstream();
      //void  getDefaultParameters(Parameters& iParameters) const;
   protected:
      float getCdfCore(float iX, const Ensemble& iEnsemble, const Parameters& iParameters) const;
      //BaseDistribution* mBaseDistribution;
      void updateParametersCore(const std::vector<Ensemble>& iEnsemble, const std::vector<Obs>& iObs, Parameters& iParameters) const {}
};
#endif
