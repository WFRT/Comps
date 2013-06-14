#ifndef CORRECTOR_CLIM_H
#define CORRECTOR_CLIM_H
#include "Corrector.h"

//! Date: 20130506
//! Author: Thomas Nipen
//! Moderates ensemble members towards the climatological mean
class CorrectorClim : public Corrector {
   public:
      CorrectorClim(const Options& iOptions, const Data& iData);
   protected:
      void getDefaultParametersCore(Parameters& iParameters) const;
      void correctCore(const Parameters& iParameters, Ensemble& iUnCorrected) const;
      void updateParametersCore(const std::vector<Ensemble>& iUnCorrected,
            const std::vector<Obs>& iObs,
            Parameters& iParameters) const;
};
#endif
