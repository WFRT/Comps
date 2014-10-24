#ifndef CORRECTOR_CONDITIONAL_H
#define CORRECTOR_CONDITIONAL_H
#include "Corrector.h"
#include "../Interpolators/Interpolator.h"

//! Corrects forecasts by ensuring that the observations and forecasts have the same marginal
class CorrectorConditional : public Corrector {
   public:
      CorrectorConditional(const Options& iOptions, const Data& iData);
   private:
      void correctCore(const Parameters& iParameters, Ensemble& iUnCorrected) const;
      void getDefaultParametersCore(Parameters& iParameters) const;
      void updateParametersCore(const std::vector<Ensemble>& iUnCorrected,
            const std::vector<Obs>& iObs,
            Parameters& iParameters) const;
      std::vector<float> mEdges;
      std::vector<float> mCenters;
      Interpolator* mInterpolator;
};
#endif
