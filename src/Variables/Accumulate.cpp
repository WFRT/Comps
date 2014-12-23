#include "Accumulate.h"
#include "../Data.h"

VariableAccumulate::VariableAccumulate(const Options& iOptions, const Data& iData) : Variable(iOptions, iData),
      mTimeWindow(Global::MV) {
   // Which base variable should be accumulated?
   iOptions.getRequiredValue("baseVariable", mBaseVariable);
   //! Over how many hours should the variable be accumulated?
   //! Defaults to the beginning of the forecast period
   iOptions.getValue("timeWindow", mTimeWindow);

   loadOptionsFromBaseVariable();
   iOptions.check();
}

float VariableAccumulate::computeCore(int iDate,
                    int iInit,
                    float iOffset,
                    const Location& iLocation,
                    const Member& iMember,
                    Input::Type iType) const {

   float startOffset = 0;
   if(Global::isValid(mTimeWindow)) {
      if(iType == Input::typeObservation) {
         // Allowed to be negative
         startOffset = iOffset - mTimeWindow;
      }
      if(Global::isValid(mTimeWindow) && iOffset > mTimeWindow) {
         startOffset = iOffset - mTimeWindow;
      }
   }

   std::string dataset = iMember.getDataset();
   std::vector<float> offsets = mData.getInput(dataset)->getOffsets();
   float total = 0;
   for(int i = 0; i < offsets.size(); i++) {
      float offset = offsets[i];
      if(offset > startOffset && offset <= iOffset) {
         float value = mData.getValue(iDate, iInit, offset, iLocation, iMember, mBaseVariable);
         if(Global::isValid(value))
            total += value;
         else
            return Global::MV;
      }
   }

   return total;
}

std::string VariableAccumulate::getBaseVariable() const {
   // TODO:
   std::stringstream ss;
   if(Global::isValid(mTimeWindow))
      ss << mBaseVariable << "_" << (int) mTimeWindow;
   else
      ss << mBaseVariable << "_Acc";
   return ss.str();
}
