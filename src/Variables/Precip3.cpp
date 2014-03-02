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
   if(input->hasVariable("PrecipAcc")) {
      float currAccumulation = iData.getValue(iDate, iInit, iOffset, iLocation, iMember, "PrecipAcc");
      int   date = Global::getDate(iDate, iInit, iOffset-3);
      float offset = Global::getOffset(iDate, iOffset-3);
      float prevAccumulation = iData.getValue(date, iInit, offset, iLocation, iMember, "PrecipAcc");
      if(!Global::isValid(prevAccumulation) || !Global::isValid(currAccumulation))
         return Global::MV;

      // Assume that if accumulation went down, that the accumation was reset (once a year?)
      if(prevAccumulation > currAccumulation)
         return Global::MV;
      return currAccumulation - prevAccumulation;
   }
   else if(input->hasVariable("Precip")) {
      float dt = 1;
      std::vector<float> offsets = input->getOffsets();
      float accum   = 0;
      float totalHours = 0;
      if(iType == Input::typeObservation) {

         iDate = Global::getDate(iDate, iInit, iOffset);
         iOffset = Global::getOffset(iDate, iOffset);
         int N = offsets.size();
         for(int i = 0; i < N; i++) {
            offsets.push_back(offsets[i]-24);
         }

         for(int i = 0; i < offsets.size(); i++) {
            if(offsets[i] > iOffset-3 && offsets[i] <= iOffset) {
               float offset = offsets[i];
               Ensemble ens = iData.getEnsemble(iDate, iInit, offset, iLocation, "Precip", iType);
               float pcp = ens[iMember.getId()];
               if(Global::isValid(pcp)) {
                  accum += pcp;
                  totalHours += 1;
               }
            }
         }
      }
      else {
         int iStart = 0;
         if(iOffset > 3) {
            iStart = iOffset - 3;
         }
         // Loop over offsets to accumulate precip within 3h period
         // The problem is that a 1h window might not be fully within the 3h window
         // so adjustments have to be made
         // Shouldn't be a problem for 3h
         for(int i = 0; i < offsets.size(); i++) {
            if(offsets[i] > iStart && offsets[i] <= (iOffset + dt)) {
               float offset = offsets[i];
               Ensemble ens = iData.getEnsemble(iDate, iInit, offset, iLocation, "Precip", iType);
               float pcp = ens[iMember.getId()];
               if(Global::isValid(pcp)) {
                  accum += pcp;
                  totalHours += 1;
               }
            }
         }
      }
      if(totalHours > 0) {
         // Normalize by the fact that we might have accumulated 24h of precip
         return accum * 3 / totalHours;
      }
      else
         return Global::MV;
   }
   else {
      std::stringstream ss;
      ss << "Cannot compute Precip3 for dataset " << iMember.getDataset()
         << " because it does not contain any suitable precip variables to accumulate over";
      Global::logger->write(ss.str(), Logger::error);
      return Global::MV;
   }
}
