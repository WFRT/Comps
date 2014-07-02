#include "TMin24.h"
#include "../Data.h"
#include "../Ensemble.h"
#include "../Member.h"

VariableTMin24::VariableTMin24() : Variable("TMin24") {}

float VariableTMin24::computeCore(const Data& iData,
      int iDate,
      int iInit,
      float iOffset,
      const Location& iLocation,
      const Member& iMember,
      Input::Type iType) const {

   Input* input = iData.getInput(iMember.getDataset());
   std::vector<float> offsets = input->getOffsets();

   bool useBch = true;

   int numMembers = input->getNumMembers();
   assert(numMembers > iMember.getId());

   float min   = Global::MV;

   // BCHydro defines TMin24 to be the 12Z forecast
   if(useBch) {
      Ensemble ens;
      // Find closest offset
      float offset = Global::INF;
      for(int i = 0; i < offsets.size(); i++) {
         // Only use 00Z forecasts
         if(fmod(offsets[i]+12, 24) == 0) {
            if(offsets[i] < iOffset) {
               if(fabs(offsets[i] - iOffset) < fabs(offset - iOffset)) {
                  offset = offsets[i];
               }
            }
         }
      }
      //std::cout << iOffset << " " << offset << std::endl;

      if(Global::isValid(offset)) {
         ens = iData.getEnsemble(iDate, iInit, offset, iLocation, "T", iType);
         min = ens.getMoment(1);
      }
   }
   else {

      int iStart = 0;
      if(iOffset > 24) {
         iStart = iOffset - 24;
      }

      for(int i = 0; i < offsets.size(); i++) {
         if(offsets[i] >= iStart && offsets[i] < iOffset) {
            float offset = offsets[i];
            Ensemble ens = iData.getEnsemble(iDate, iInit, offset, iLocation, "T", iType);
            float value;
            if(ens.size() == 1) {
               value = ens[0];
            }
            else {
               int index = round((float) iMember.getId() / (numMembers-1) * (ens.size()-1));
               assert(index >=0 && index <= ens.size());
               value = ens[index];
            }
            if(Global::isValid(value)) {
               if(!Global::isValid(min) || value < min) {
                  min = value;
               }
            }
         }
      }
   }
   
   return min;
}
