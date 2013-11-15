#include "Precip3.h"
#include "../Data.h"
#include "../Ensemble.h"
#include "../Member.h"

VariablePrecip3::VariablePrecip3() : Variable("Precip3") {}

float VariablePrecip3::computeCore(const Data& iData,
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
   std::string precipVar = "PrecipAcc";
   float dt;
   if(!input->hasVariable(precipVar)) {
      std::stringstream ss;
      ss << "Cannot compute Precip3 for dataset " << iMember.getDataset()
         << " because it does not contain varuake 'PrecipAcc'";
      Global::logger->write(ss.str(), Logger::error);
   }
   float currAccumulation = iData.getValue(iDate, iInit, iOffset, iLocation, iMember, precipVar);
   int   date = Global::getDate(iDate, iInit, iOffset-3);
   float offset = Global::getOffset(iDate, iOffset-3);
   float prevAccumulation = iData.getValue(date, iInit, offset, iLocation, iMember, precipVar);
   if(!Global::isValid(prevAccumulation) || !Global::isValid(currAccumulation))
      return Global::MV;

   // Assume that if accumulation went down, that the accumation was reset (once a year?)
   if(prevAccumulation > currAccumulation)
      return Global::MV;
   //std::cout << "Offset: " << iOffset << " prevOffset: " << offset << "";
   //std::cout << "Previous: " << prevAccumulation << " current: " << currAccumulation << std::endl;
   return currAccumulation - prevAccumulation;
}
