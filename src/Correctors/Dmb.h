#ifndef CORRECTOR_DMB_H
#define CORRECTOR_DMB_H
#include "Corrector.h"

//! @date 20130305
//! @author Dominique Bourdin
//! Bias-correction using degree of mass balance
class CorrectorDmb : public Corrector {
   public:
      CorrectorDmb(const Options& iOptions, const Data& iData);
   private:
      void getDefaultParametersCore(Parameters& iParameters) const;
      int numParametersCore() const {return 2;};
      void correctCore(const Parameters& iParameters, Ensemble& iUnCorrected) const;
      void updateParametersCore(const std::vector<Ensemble>& iUnCorrected,
            const std::vector<Obs>& iObs,
            Parameters& iParameters) const;
      float mFixedDivisor;
};
#endif
