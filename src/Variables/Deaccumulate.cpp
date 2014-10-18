#include "Deaccumulate.h"
#include "../Data.h"

VariableDeaccumulate::VariableDeaccumulate(const Options& iOptions, const Data& iData) : Variable(iOptions, iData),
      mTimeWindow(1) {

   // Which base variable should be deaccumulated?
   iOptions.getRequiredValue("baseVariable", mBaseVariable);
   //! Over how many hours should the variable be accumulated?
   //! Defaults to the beginning of the forecast period
   iOptions.getValue("timeWindow", mTimeWindow);

   const Variable* var = Variable::get(VariableDeaccumulate::getBaseVariable());
   mLowerLimit = var->getMin();
   mUpperLimit = var->getMax();

   loadOptionsFromBaseVariable();
   iOptions.check();
}

float VariableDeaccumulate::computeCore(int iDate,
                    int iInit,
                    float iOffset,
                    const Location& iLocation,
                    const Member& iMember,
                    Input::Type iType) const {

   if(iOffset < mTimeWindow)
      return 0;
   float startOffset = iOffset - mTimeWindow;

   float prevAccumulation = mData.getValue(iDate, iInit, startOffset, iLocation, iMember, mBaseVariable);
   float currAccumulation = mData.getValue(iDate, iInit, iOffset, iLocation, iMember, mBaseVariable);

   if(Global::isValid(prevAccumulation) && Global::isValid(currAccumulation)) {
      float diff = currAccumulation - prevAccumulation;
      if(Global::isValid(mLowerLimit) && diff < mLowerLimit) {
         std::stringstream ss;
         ss << "VariableDeaccumulate: Deaccumulated " << getBaseVariable() << " is lower than the variable's "
            << "theoretical limit of " << mLowerLimit;
         Global::logger->write(ss.str(), Logger::debug);
         diff = 0;
      }
      return diff;
   }
   else
      return Global::MV;
}

std::string VariableDeaccumulate::getBaseVariable() const {
   // e.g. Precip
   std::string undecorated = Variable::getUndecoratedVariable(mBaseVariable);

   if((int) mTimeWindow != 1) {
      std::stringstream ss;
      ss << undecorated << "_Acc" << (int) mTimeWindow;
      return ss.str();
   }
   else
      return undecorated;
}
