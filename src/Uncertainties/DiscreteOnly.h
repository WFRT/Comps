#ifndef UNCERTAINTY_DISCRETE_ONLY_H
#define UNCERTAINTY_DISCRETE_ONLY_H
#include "Uncertainty.h"

// Useful for uncertainty models containing only a descrete part (like POP: probability of
// precipitation)
class UncertaintyDiscreteOnly : public Uncertainty {
   public:
      UncertaintyDiscreteOnly(const Options& iOptions, const Data& iData);
      ~UncertaintyDiscreteOnly();
      void getDistribution(const Ensemble& iEnsemble, const Parameters& iParameters, Distribution& iDistribution);
      void updateParameters(const Ensemble& iEnsemble, const Obs& iObs, Parameters& iParameters) const;
      void getDefaultParameters(Parameters& iParameters) const;
      bool needsConstantEnsembleSize() const;
   protected:
      float getCdf(float iX, const Ensemble& iEnsemble, const Parameters& iParameters) const;
      float getPdf(float iX, const Ensemble& iEnsemble, const Parameters& iParameters) const;
      //! Returns 0 (false) if cdf is less than POP and 1 (true) otherwise
      float getInv(float iCdf, const Ensemble& iEnsemble, const Parameters& iParameters) const;
      float getMoment(int iMoment, const Ensemble& iEnsemble, const Parameters& iParameters) const;
      Discrete*   mDiscrete;
};
#endif

