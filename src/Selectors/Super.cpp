#include "Super.h"
#include "../Inputs/Input.h"
#include "../Member.h"
#include "../Data.h"
#include "../Slice.h"


SelectorSuper::SelectorSuper(const Options& iOptions, const Data& iData) : Selector(iOptions, iData) {
   // Get all the ensembles
   std::vector<std::string> datasets;
   //! Tags of inputs used to create ensemble 
   iOptions.getRequiredValues("datasets", datasets);

   // Store the members ahead of time, so that it only is done once
   for(int i = 0; i < datasets.size(); i++) {
      Input* input = mData.getInput(datasets[i]);
      mInputs.push_back(input);

      std::vector<Member> members;
      input->getMembers(members);
      // Add all members of this dataset
      for(int m = 0; m < members.size(); m++) {
         mMembers.push_back(members[m]);
      }
   }
}
void SelectorSuper::selectCore(int iDate,
      int iInit,
      float iOffset,
      const Location& iLocation,
      const std::string& iVariable,
      const Parameters& iParameters,
      std::vector<Slice>& iSlices) const {

   // Add all members of this dataset
   for(int m = 0; m < mMembers.size(); m++) {
      Slice slice(iDate, iInit, iOffset, mMembers[m]);
      iSlices.push_back(slice);
   }
}
