#include "Member.h"
#include "../Member.h"
#include "../Data.h"
#include "../Slice.h"

SelectorMember::SelectorMember(const Options& iOptions, const Data& iData) :
      Selector(iOptions, iData),
      mMaxResolution(Global::MV),
      mMinResolution(Global::MV),
      mSelectByMember(false) {
   if(iOptions.getValues("members", mMembers)) {
      mSelectByMember = true;
   }
   iOptions.getValue("maxResolution", mMaxResolution);
   iOptions.getValue("minResolution", mMinResolution);
   iOptions.getValues("models", mModels);
}

void SelectorMember::selectCore(int iDate,
      int iInit,
      float iOffset,
      const Location& iLocation,
      const std::string& iVariable,
      const Parameters& iParameters,
      std::vector<Slice>& iSlices) const {

   std::vector<Member> members;
   mData.getInput()->getMembers(members);
   std::string datasetName = mData.getInput()->getName();

   std::vector<int> memberIds;
   if(mSelectByMember) {
      memberIds = mMembers;
   }
   else {
      for(int i = 0; i < (int) members.size(); i++) {
         memberIds.push_back(i);
      }
   }
   for(int i = 0; i < (int) memberIds.size(); i++) {
      int id = memberIds[i];
      float resolution = members[id].getResolution();
      assert(id < (int) members.size());
      // Check that model name matches
      if(mModels.size() == 0 || find(mModels.begin(), mModels.end(), members[id].getModel())!= mModels.end()) {
         // Check that resolution matches
         if((mMaxResolution == Global::MV && mMinResolution == Global::MV) ||
               (mMaxResolution != Global::MV && resolution <= mMaxResolution &&
                mMinResolution != Global::MV && resolution >= mMinResolution) ||
               (mMaxResolution != Global::MV && resolution <= mMaxResolution &&
                mMinResolution == Global::MV ) ||
               (mMaxResolution == Global::MV   && 
                resolution <= mMaxResolution)) {

            Slice slice(iDate, iInit, iOffset, members[id]);
            iSlices.push_back(slice);
         }
      }
   }
}

int SelectorMember::getMaxMembers() const {
   return (int) mMembers.size();
}
