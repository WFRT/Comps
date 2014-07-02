#include "Precip6.h"
#include "../Data.h"
#include "../Ensemble.h"
#include "../Member.h"

VariablePrecip6::VariablePrecip6() : Variable("Precip6") {}

float VariablePrecip6::computeCore(const Data& iData,
      int iDate,
      int iInit,
      float iOffset,
      const Location& iLocation,
      const Member& iMember,
      Input::Type iType) const {
   Input* input;
   if(iMember.getDataset() != "") {
      input = iData.getInput(iMember.getDataset());
   }
   else {
      input = iData.getInput("Precip", iType);
   }
   std::vector<float> offsets = input->getOffsets();

   if(iType == Input::typeForecast && iOffset < 6) {
      // Cannot compute accum precip for forecast
      return Global::MV;
   }

   if(iType == Input::typeObservation) {
      iDate   = Global::getDate(iDate, iInit, iOffset);
      iOffset = Global::getTime(iDate, iInit, iOffset);
      assert(iOffset >= 0);
      assert(iOffset < 24);
   }
   int iStart = iOffset -6;

   // Determine offsets to pull
   std::vector<float> useOffsets;
   for(int i = 0; i < offsets.size(); i++) {
      float currOffset = offsets[i];
      if(iType == Input::typeObservation) {
         if((currOffset >= iStart && currOffset < iOffset)) {
            useOffsets.push_back(currOffset);
         }
         else if(currOffset - 24 >= iStart && currOffset - 24 < iOffset) {
            // Yesterday's offset
            useOffsets.push_back(currOffset-24);
         }
      }
      else {
         if(currOffset >= iStart && currOffset < iOffset) {
            useOffsets.push_back(currOffset);
         }
      }
   }


   iStart = iOffset - 6;
   float accum   = 0;
   int   counter = 0;
   for(int i = 0; i < useOffsets.size(); i++) {
      float value = iData.getValue(iDate, iInit, useOffsets[i], iLocation, iMember, "Precip");
      if(Global::isValid(value)) {
         accum += value;
         counter++;
      }
   }
   if(counter == 0)
      return Global::MV;
   
   float total = (float) accum / counter * 6;
   return total;
}
