#include "Precip24.h"
#include "../Data.h"
#include "../Ensemble.h"
#include "../Member.h"

VariablePrecip24::VariablePrecip24() : Variable("Precip24") {}

float VariablePrecip24::computeCore(const Data& iData,
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
   float dt;
   if(input->hasVariable("Precip")) {
      precipVar = "Precip";
      dt = 1;
   }
   else if(input->hasVariable("Precip6")) {
      precipVar = "Precip6";
      dt = 6;
   }
   else {
      std::stringstream ss;
      ss << "Cannot compute Precip24 for dataset " << iMember.getDataset()
         << " because it does not contain any suitable precip variables to accumulate over";
      Global::logger->write(ss.str(), Logger::error);
      dt = Global::MV;
   }

   std::vector<float> offsets = input->getOffsets();
   int iStart = 0;
   if(iOffset > 24) {
      iStart = iOffset - 24;
   }
   // Loop over offsets to accumulate precip within 24h period
   // The problem is that a 6h window might not be fully within the 24h window
   // so adjustments have to be made
   float accum   = 0;
   float totalHours = 0;
   for(int i = 0; i < offsets.size(); i++) {
      if(offsets[i] > iStart && offsets[i] <= (iOffset + dt)) {
         float offset = offsets[i];
         Ensemble ens = iData.getEnsemble(iDate, iInit, offset, iLocation, precipVar, iType);
         float pcp = ens[iMember.getId()];
         if(Global::isValid(pcp)) {
            float pcpHourly = pcp / dt; // Get an hourly rate
            // Compute how many hours of current offset is within 24h window
            float hours = dt;
            if(offset - iStart < dt) {
               hours = offset-iStart;
            }
            else if(offset > iOffset && iOffset - (offset-dt)< dt) {
               hours = iOffset - (offset-dt);
            }
            pcp = pcpHourly * hours;
            accum += pcp;
            totalHours += hours;
         }
      }
   }
   if(totalHours > 0) {
      // Normalize by the fact that we might have accumulated 24h of precip
      return accum * 24 / totalHours;
   }
   else
      return Global::MV;
}
