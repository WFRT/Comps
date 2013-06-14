#include "Fixed.h"
#include "../Data.h"
#include "../Obs.h"
#include "../Parameters.h"
#include "../Ensemble.h"
CorrectorFixed::CorrectorFixed(const Options& iOptions, const Data& iData) :
      Corrector(iOptions, iData),
      mRand(boost::mt19937(0), boost::normal_distribution<>()),
      mValue(0) {
   iOptions.getValue("value", mValue);
}
void CorrectorFixed::correctCore(const Parameters& iParameters,
      Ensemble& iUnCorrected) const {
   for(int i = 0; i < iUnCorrected.size(); i++) {
      iUnCorrected[i] = mValue;
   }
}

void CorrectorFixed::getDefaultParametersCore(Parameters& iParameters) const {
   std::vector<float> coeffs;
   coeffs.push_back(mValue);
   iParameters.setAllParameters(coeffs);
}

void CorrectorFixed::updateParametersCore(const std::vector<Ensemble>& iUnCorrected,
      const std::vector<Obs>& iObs,
      Parameters& iParameters) const {
}
