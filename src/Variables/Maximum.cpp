#include "Maximum.h"
#include "../Data.h"

VariableMaximum::VariableMaximum(const Options& iOptions, const Data& iData) : Variable(iOptions, iData),
      mTimeWindow(Global::MV) {
   // Which base variable should be accumulated?
   iOptions.getRequiredValue("baseVariable", mBaseVariable);
   //! How many hours back in time should the maximum be computed for?
   //! Defaults to since the beginning of the forecast period
   iOptions.getValue("timeWindow", mTimeWindow);

   loadOptionsFromBaseVariable();
   iOptions.check();
}

float VariableMaximum::computeCore(int iDate,
                    int iInit,
                    float iOffset,
                    const Location& iLocation,
                    const Member& iMember,
                    Input::Type iType) const {

   float startOffset = 0;
   if(Global::isValid(mTimeWindow) && iOffset > mTimeWindow) 
      startOffset = iOffset - mTimeWindow;

   std::vector<float> offsets = mData.getInput()->getOffsets();
   float max = Global::MV;
   for(int i = 0; i < offsets.size(); i++) {
      float offset = offsets[i];
      if(offset > startOffset && offset <= iOffset) {
         float value = mData.getValue(iDate, iInit, offset, iLocation, iMember, mBaseVariable);
         if(!Global::isValid(max))
            max = value;
         else if(Global::isValid(value) && value > max)
            max = value;
      }
   }
   return max;
}

std::string VariableMaximum::getBaseVariable() const {
   // TODO:
   std::stringstream ss;
   if(Global::isValid(mTimeWindow))
      ss << mBaseVariable << "_Max" << (int) mTimeWindow;
   else
      ss << mBaseVariable << "_Max";
   return ss.str();
}
