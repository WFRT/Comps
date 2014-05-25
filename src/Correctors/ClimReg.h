#ifndef CORRECTOR_CLIM_REG_H
#define CORRECTOR_CLIM_REG_H
#include "Corrector.h"

//! @date 20140523
//! @author Thomas Nipen
//! Uses linear regression between forecast and climatology
class CorrectorClimReg : public Corrector {
   public:
      CorrectorClimReg(const Options& iOptions, const Data& iData);
   private:
      void getDefaultParametersCore(Parameters& iParameters) const;
      void correctCore(const Parameters& iParameters, Ensemble& iUnCorrected) const;
      void updateParametersCore(const std::vector<Ensemble>& iUnCorrected,
            const std::vector<Obs>& iObs,
            Parameters& iParameters) const;
};
#endif
