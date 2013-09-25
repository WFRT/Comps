#ifndef CORRECTOR_RECENT_OBS_H
#define CORRECTOR_RECENT_OBS_H
#include "Corrector.h"

//! Date: 20130923
//! Author: Thomas Nipen
//! Use the most recent observation to bias-correct forecast
//! NOTE: If multiple correctors are chained, this must be first
class CorrectorRecentObs : public Corrector {
   public:
      CorrectorRecentObs(const Options& iOptions, const Data& iData);
      void getDefaultParametersCore(Parameters& iParameters) const;
      void updateParametersCore(const std::vector<Ensemble>& iUnCorrected,
            const std::vector<Obs>& iObs,
            Parameters& iParameters) const;
   protected:
      void correctCore(const Parameters& iParameters, Ensemble& iUnCorrected) const;
};
#endif
