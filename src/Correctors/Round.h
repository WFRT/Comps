#ifndef CORRECTOR_ROUND_H
#define CORRECTOR_ROUND_H
#include "Corrector.h"

//! Rounds forecast value down or up to some threshold if the forecast is less than what the
//! observations historically are when the forecast is on the threshold.
//! Useful for precipitation ensembles to help discrimiate between occurrance and non-occurance
//! of precip
class CorrectorRound : public Corrector {
   public:
      CorrectorRound(const Options& iOptions, const Data& iData);
   private:
      void correctCore(const Parameters& iParameters, Ensemble& iUnCorrected) const;
      void getDefaultParametersCore(Parameters& iParameters) const;
      int  numParametersCore() const {return 1;};
      void updateParametersCore(const std::vector<Ensemble>& iUnCorrected,
            const std::vector<Obs>& iObs,
            Parameters& iParameters) const;
      float mRoundDownTo;
      float mRoundUpTo;
      float mFixed;
};
#endif
