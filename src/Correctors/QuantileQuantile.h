#ifndef CORRECTOR_QUANTILE_QUANTILE_H
#define CORRECTOR_QUANTILE_QUANTILE_H
#include "Corrector.h"

//! @date 20130928
//! @author Thomas Nipen
//! Corrects forecasts by ensuring that the observations and forecasts have the same marginal
//! distribution
class CorrectorQuantileQuantile : public Corrector {
   public:
      CorrectorQuantileQuantile(const Options& iOptions, const Data& iData);
   private:
      void getDefaultParametersCore(Parameters& iParameters) const;
      void correctCore(const Parameters& iParameters, Ensemble& iUnCorrected) const;
      void updateParametersCore(const std::vector<Ensemble>& iUnCorrected,
            const std::vector<Obs>& iObs,
            Parameters& iParameters) const;
      // Create sorted observation and forecast quantiles from parameters
      static void getSortedQuantiles(const Parameters& iParameters, std::vector<float>& iObsQuantiles, std::vector<float>& iFcstQuantiles);
      int mMaxPoints;
};
#endif
