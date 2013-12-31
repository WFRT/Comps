#include "Fixed.h"
#include "../Data.h"
#include "../Obs.h"
#include "../Parameters.h"
#include "../Ensemble.h"
CorrectorFixed::CorrectorFixed(const Options& iOptions, const Data& iData) :
      Corrector(iOptions, iData) {
   //! Set ensemble members to this value
   iOptions.getRequiredValue("value", mValue);
}
void CorrectorFixed::correctCore(const Parameters& iParameters,
      Ensemble& iUnCorrected) const {
   for(int i = 0; i < iUnCorrected.size(); i++) {
      iUnCorrected[i] = mValue;
   }
}
