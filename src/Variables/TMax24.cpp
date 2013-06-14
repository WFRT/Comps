#include "TMax24.h"
#include "../Data.h"
#include "../Ensemble.h"
#include "../Member.h"

VariableTMax24::VariableTMax24() : Variable("TMax24") {}

float VariableTMax24::computeCore(const Data& iData,
      int iDate,
      int iInit,
      float iOffset,
      const Location& iLocation,
      const Member& iMember,
      Input::Type iType) const {

   std::vector<float> offsets;
   Input* input = iData.getInput(iMember.getDataset());
   input->getOffsets(offsets);

   bool useBch = true;

   int numMembers = input->getNumMembers();
   assert(numMembers > iMember.getId());

   float max   = Global::MV;

   // BCHydro defines TMax24 to be the 24Z forecast
   if(useBch) {
      Ensemble ens;
      // Find closest offset
      float offset = Global::INF;
      for(int i = 0; i < offsets.size(); i++) {
         // Only use 00Z forecasts
         if(fmod(offsets[i], 24) == 0) {
            if(fabs(offsets[i] - iOffset) < fabs(offset - iOffset)) {
               offset = offsets[i];
            }
         }
      }
      //std::cout << iOffset << " " << offset << std::endl;

      if(Global::isValid(offset)) {
         iData.getEnsemble(iDate, iInit, offset, iLocation, iMember.getDataset(), "T", ens);
         max = ens.getMoment(1);
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
            Ensemble ens;
            iData.getEnsemble(iDate, iInit, offset, iLocation, iMember.getDataset(), "T", ens);
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
               if(!Global::isValid(max) || value > max) {
                  max = value;
               }
            }
         }
      }
   }
   
   return max;
}
