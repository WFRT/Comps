#ifndef CONTINUOUS_GAMMA_H
#define CONTINUOUS_GAMMA_H
#include "../Global.h"
#include "../Options.h"
#include "Continuous.h"
class BaseDistribution;

/** NOTE: Assuming individual first moments, and pooled second moments */
class ContinuousGamma : public Continuous {
   public:
      ContinuousGamma(const Options& iOptions, const Data& iData);
      ~ContinuousGamma();
   private:
      float getCdfCore(float iX,   const Ensemble& iEnsemble, const Parameters& iParameters) const;
      float getPdfCore(float iX,   const Ensemble& iEnsemble, const Parameters& iParameters) const;
      float getInvCore(float iCdf, const Ensemble& iEnsemble, const Parameters& iParameters) const;
      BaseDistribution* mBaseDistribution;
      //void updateParametersCore(const std::vector<Ensemble>& iEnsemble, const std::vector<Obs>& iObs, Parameters& iParameters) const;
      bool getMoments(const Ensemble& iEnsemble, const Parameters& iParameters, std::vector<float>& iMoments) const;
      void getDefaultParametersCore(Parameters& iParameters) const;
      int numParametersCore() const {return 2;};
      bool mUseConst;
      bool mUseMean;
};
#endif
