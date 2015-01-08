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

   float total = 0;
   ///////////////////////////////
   // Accumulating observations //
   ///////////////////////////////
   // When accumulating observations, we are allowed to look at yesterday's values.
   // This can happen for low offsets, where the accumulation window crosses into yesterday
   if(iType == Input::typeObservation) {
      float startOffset = iOffset - mTimeWindow; // Allowed to be negative
      std::string dataset = iMember.getDataset();
      std::vector<float> offsets = mData.getInput(dataset)->getOffsets();
      for(int i = 0; i < offsets.size(); i++) {
         float offset = offsets[i];
         if(offset < 24) { // Observations should never have offsets above 24, but just in case
            // Use a value from today
            if(offset > startOffset && offset <= iOffset) {
               float value = mData.getValue(iDate, iInit, offset, iLocation, iMember, mBaseVariable);
               if(Global::isValid(value))
                  total += value;
               else
                  return Global::MV;
            }
            // Use a value from yesterday
            else if(offset > startOffset+24 && offset <= iOffset + 24) {
               int date = Global::getDate(iDate, -24); // yesterday
               float value = mData.getValue(date, iInit, offset, iLocation, iMember, mBaseVariable);
               if(Global::isValid(value))
                  total += value;
               else
                  return Global::MV;
            }
         }
      }
   }
   ////////////////////////////
   // Accumulating forecasts //
   ////////////////////////////
   // Only accumulate values for forecasts valid at this initialization date/time
   else {
      if(Global::isValid(mTimeWindow) && iOffset > mTimeWindow) {
         // Do a regular sum between start and end offsets
         float startOffset = iOffset - mTimeWindow;
         std::string dataset = iMember.getDataset();
         std::vector<float> offsets = mData.getInput(dataset)->getOffsets();
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
      }
      else {
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
