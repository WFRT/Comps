#ifndef CORRECTOR_BEST_H
#define CORRECTOR_BEST_H
#include "Corrector.h"

//! Reducte the size of the ensemble such that it only contains the best members.
//! Based on the mean absolute error of each member.
class CorrectorBest : public Corrector {
   public:
      CorrectorBest(const Options& iOptions, const Data& iData);
   private:
      void getDefaultParametersCore(Parameters& iParameters) const;
      void correctCore(const Parameters& iParameters, Ensemble& iUnCorrected) const;
      void updateParametersCore(const std::vector<Ensemble>& iUnCorrected,
            const std::vector<Obs>& iObs,
            Parameters& iParameters) const;
      bool needsConstantEnsembleSize() const {return true;};
      int mNum;
};
#endif
