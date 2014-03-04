#ifndef CORRECTOR_CLIM_H
#define CORRECTOR_CLIM_H
#include "Corrector.h"

//! @date 20130506
//! @author Thomas Nipen
//! Moderates ensemble members towards the climatological mean
class CorrectorClim : public Corrector {
   public:
      CorrectorClim(const Options& iOptions, const Data& iData);
   private:
      void getDefaultParametersCore(Parameters& iParameters) const;
      void correctCore(const Parameters& iParameters, Ensemble& iUnCorrected) const;
      void updateParametersCore(const std::vector<Ensemble>& iUnCorrected,
            const std::vector<Obs>& iObs,
            Parameters& iParameters) const;
      bool mComputeClim;
};
#endif
