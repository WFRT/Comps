#ifndef CORRECTOR_POLYNOMIAL_REGRESSION_H
#define CORRECTOR_POLYNOMIAL_REGRESSION_H
#include "Corrector.h"

class CorrectorPolynomialRegression : public Corrector {
   public:
      CorrectorPolynomialRegression(const Options& iOptions, const Data& iData);
   protected:
      void getDefaultParametersCore(Parameters& iParameters) const;
      void correctCore(const Parameters& iParameters, Ensemble& iUnCorrected) const;
      void updateParametersCore(const std::vector<Ensemble>& iUnCorrected,
            const std::vector<Obs>& iObs,
            Parameters& iParameters) const;
      void computeCoefficients(const Parameters& iParameters,
            std::vector<float>& iCoefficients) const;
   private:
      int mOrder;
      std::vector<bool> mUseOrders;
};
#endif
