#include "Averager.h"
#include "Member.h"

AveragerMember::AveragerMember(const Options& iOptions, const Data& iData) : Averager(iOptions, iData) {
   iOptions.getRequiredValues("members", mMembers);
}

float AveragerMember::average(const Ensemble& iValues, const Parameters& iParameters) const {
   float mean = 0;
   int counter = 0;
   for(int i = 0; i < (int) mMembers.size(); i++) {
      int member = mMembers[i];
      assert(member < (int) iValues.size());
      if(iValues[member] != Global::MV) {
         mean += iValues[member];
         counter++;
      }
   }
   if(counter != 0) {
      mean = mean / counter;
   }
   else  {
      mean = Global::MV;
   }
   return mean;
}
