#ifndef CONTINUOUS_BPE_H
#define CONTINUOUS_BPE_H
#include "../Global.h"
#include "Continuous.h"
#include "../Scheme.h"
#include "../Interpolators/Interpolator.h"

//! Binned probability ensemble, where the cumulative probability is given by the fraction of
//! ensemble members below. The probability mass between two consecutive ensemble members is fixed
//! and equals 1/(numMembers+1). Currently, the CDF is set to 0 and 1 below and above the ensemble,
//! respectively.
class ContinuousBpe : public Continuous {
   public:
      ContinuousBpe(const Options& iOptions, const Data& iData);
      ~ContinuousBpe();
      bool needsTraining() const {return false;};
   private:
      float getCdfCore(float iX, const Ensemble& iEnsemble, const Parameters& iParameters) const;
      float getPdfCore(float iX, const Ensemble& iEnsemble, const Parameters& iParameters) const;
      float getInvCore(float iCdf, const Ensemble& iEnsemble, const Parameters& iParameters) const;
      void  updateParametersCore(const std::vector<Ensemble>& iEnsemble, const std::vector<Obs>& iObs, Parameters& iParameters) const {};
      Interpolator* mInterpolator;
      void getXY(const Ensemble& iEnsemble, std::vector<float>& iX, std::vector<float>& iY) const;
      bool mHandleOutside;
      bool mUseStep;
};
#endif
