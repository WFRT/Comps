#ifndef CORRECTOR_MULTI_VARIATE_REGRESSION_H
#define CORRECTOR_MULTI_VARIATE_REGRESSION_H
#include "Corrector.h"

class CorrectorMultiVariateRegression : public Corrector {
   public:
      CorrectorMultiVariateRegression(const Options& iOptions, const Data& iData);
   private:
      void getDefaultParametersCore(Parameters& iParameters) const;
      int numParametersCore() const;
      void correctCore(const Parameters& iParameters, Ensemble& iUnCorrected) const;
      void updateParametersCore(const std::vector<Ensemble>& iUnCorrected,
            const std::vector<Obs>& iObs,
            Parameters& iParameters) const;
      std::vector<std::string> mVariables;
};
#endif
