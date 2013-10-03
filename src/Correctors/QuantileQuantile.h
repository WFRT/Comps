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
   protected:
      void getDefaultParametersCore(Parameters& iParameters) const;
      void correctCore(const Parameters& iParameters, Ensemble& iUnCorrected) const;
      void updateParametersCore(const std::vector<Ensemble>& iUnCorrected,
            const std::vector<Obs>& iObs,
            Parameters& iParameters) const;
      void getSortedQuantiles(const Parameters& iParameters, std::vector<float>& iObsQuantiles, std::vector<float>& iFcstQuantiles) const;
   private:
      int mMaxPoints;
      bool mCorrectExtremes;
};
#endif
