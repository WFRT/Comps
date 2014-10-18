#include "Fraction.h"
#include "../Data.h"
#include "../Member.h"

VariableFraction::VariableFraction(const Options& iOptions, const Data& iData) : Variable(iOptions, iData) {
   //! Which variable should be used as temperature?
   iOptions.getValue("fractionVariable", mFractionVariable);
   iOptions.getValue("totalVariable", mTotalVariable);

   loadOptionsFromBaseVariable();
   iOptions.check();
}

float VariableFraction::computeCore(int iDate,
      int iInit,
      float iOffset,
      const Location& iLocation,
      const Member& iMember,
      Input::Type iType) const {

   float Total    = mData.getValue(iDate, iInit, iOffset, iLocation, iMember, mTotalVariable);
   float Fraction = mData.getValue(iDate, iInit, iOffset, iLocation, iMember, mFractionVariable);
   if(!Global::isValid(Total) || !Global::isValid(Fraction) || Total == 0)
      return Global::MV;
   else {
      float frac = Fraction / Total;
      return frac;
   }
}

std::string VariableFraction::getBaseVariable() const {
   std::stringstream ss;
   ss << mFractionVariable << "_Fraction";
   return ss.str();
}
