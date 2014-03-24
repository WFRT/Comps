#include "Fixed.h"
#include "../Data.h"
#include "../Obs.h"
#include "../Parameters.h"
#include "../Ensemble.h"
CorrectorFixed::CorrectorFixed(const Options& iOptions, const Data& iData) :
      Corrector(iOptions, iData),
      mValue(Global::MV),
      mAdd(Global::MV) {
   //! Set ensemble members to this value
   iOptions.getValue("value", mValue);
   //! Add this value to each member
   iOptions.getValue("add", mAdd);
   if(!Global::isValid(mValue) && !Global::isValid(mAdd)) {
      std::stringstream ss;
      ss << "CorrectorFixed: One of 'value' or 'add' must be specified.";
      Global::logger->write(ss.str(), Logger::error);
   }
}
void CorrectorFixed::correctCore(const Parameters& iParameters,
      Ensemble& iUnCorrected) const {
   for(int i = 0; i < iUnCorrected.size(); i++) {
      if(Global::isValid(mValue)) {
         iUnCorrected[i] = mValue;
      }
      else if(Global::isValid(mAdd)) {
         if(Global::isValid(iUnCorrected[i])) {
            iUnCorrected[i] += mAdd;
         }
      }
   }
}
