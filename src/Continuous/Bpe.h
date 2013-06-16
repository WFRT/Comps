#ifndef CONTINUOUS_BPE_H
#define CONTINUOUS_BPE_H
#include "../Global.h"
#include "../Options.h"
#include "../Scheme.h"
#include "../Interpolators/Interpolator.h"
class BaseDistribution;

class ContinuousBpe : public Continuous {
   public:
      //! Options
      //! * distribution: Tag of distribution to use outside ensemble
      //! * interp: Tag of interpolation scheme to use between ensemble members
      ContinuousBpe(const Options& rOptions, const Data& iData);
      ~ContinuousBpe();
      //void  getDefaultParameters(Parameters& iParameters) const;
   protected:
      float getCdfCore(float iX, const Ensemble& iEnsemble, const Parameters& iParameters) const;
      float getPdfCore(float iX, const Ensemble& iEnsemble, const Parameters& iParameters) const;
      float getInvCore(float iCdf, const Ensemble& iEnsemble, const Parameters& iParameters) const;
      void updateParametersCore(
            const Ensemble& iEnsemble,
            const Obs& iObs,
            Parameters& iParameters) const {};
      Interpolator* mInterpolator;
      BaseDistribution* mBaseDistribution;
};
#endif
