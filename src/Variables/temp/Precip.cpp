#include "Precip.h"
#include "../Data.h"
#include "../Ensemble.h"
#include "../Member.h"

VariablePrecip::VariablePrecip() : Variable("Precip") {}

float VariablePrecip::computeCore(const Data& iData,
      int iDate,
      int iInit,
      float iOffset,
      const Location& iLocation,
      const Member& iMember,
      Input::Type iType) const {
   Input* input = iData.getInput(iMember.getDataset());
   int numMembers = input->getNumMembers();
   assert(numMembers > iMember.getId());

   // Figure out which precip base variable to use
   std::string precipVar;
   if(input->hasVariable("PrecipAcc")) {
      std::vector<float> offsets = input->getOffsets();
      // Can't compute rate with only one data point
      if(offsets.size() <= 1)
         return Global::MV;

      int lowerIndex = Global::getLowerIndex(iOffset, offsets);
      int upperIndex = Global::getUpperIndex(iOffset, offsets);
      if(!Global::isValid(lowerIndex) || !Global::isValid(upperIndex))
         // Offset is below or above all offsets
         return Global::MV;

      if(lowerIndex == upperIndex) {
         if(lowerIndex == 0)
            // No accumulation defined for the first offset
            return Global::MV;
         else
            lowerIndex--;
      }

      float currOffset = offsets[upperIndex];
      float prevOffset = offsets[lowerIndex];
      float dt = currOffset - prevOffset;
      assert(dt > 0);

      float currAccumulation = iData.getValue(iDate, iInit, currOffset, iLocation, iMember, "PrecipAcc");
      float prevAccumulation = iData.getValue(iDate, iInit, prevOffset, iLocation, iMember, "PrecipAcc");
      if(!Global::isValid(prevAccumulation) || !Global::isValid(currAccumulation))
         return Global::MV;

      // Assume that if accumulation went down, that the accumation was reset (once a year?)
      if(prevAccumulation > currAccumulation)
         return 0;
      return (currAccumulation - prevAccumulation)/dt;
   }
   else {
      float dt;
      if(input->hasVariable("Precip3")) {
         precipVar = "Precip3";
         dt = 3;
      }
      else if(input->hasVariable("Precip6")) {
         precipVar = "Precip6";
         dt = 6;
      }
      else if(input->hasVariable("Precip24")) {
         precipVar = "Precip24";
         dt = 24;
      }
      else {
         std::stringstream ss;
         ss << "Cannot compute Precip for dataset " << iMember.getDataset()
            << " because it does not contain any suitable precip variables to take the"
            << " derivative of";
         Global::logger->write(ss.str(), Logger::error);
      }
      float nearestOffset = iOffset;
      float accValue = iData.getValue(iDate, iInit, nearestOffset, iLocation, iMember, precipVar);

      if(!Global::isValid(accValue))
         return Global::MV;

      return accValue / dt;
   }
}
