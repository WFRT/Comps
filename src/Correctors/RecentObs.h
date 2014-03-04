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
   private:
      void correctCore(const Parameters& iParameters, Ensemble& iUnCorrected) const;
      void getDefaultParametersCore(Parameters& iParameters) const;
      int  numParametersCore() const {return 1;};
      void updateParametersCore(const std::vector<Ensemble>& iUnCorrected,
            const std::vector<Obs>& iObs,
            Parameters& iParameters) const;
};
#endif
