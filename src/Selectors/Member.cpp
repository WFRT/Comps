#include "Member.h"
#include "../Member.h"
#include "../Data.h"
#include "../Field.h"

SelectorMember::SelectorMember(const Options& iOptions, const Data& iData) :
      Selector(iOptions, iData),
      mMaxResolution(Global::MV),
      mMinResolution(Global::MV),
      mSelectByMember(false),
      mDataset("") {
   //! Add ensemble members with these indices
   if(iOptions.getValues("members", mMembers)) {
      mSelectByMember = true;
   }
   //! Allow ensemble members with resolution <= this (in km)
   iOptions.getValue("maxResolution", mMaxResolution);
   //! Allow ensemble members with resolution >= this (in km)
   iOptions.getValue("minResolution", mMinResolution);
   //! Allow ensemble members with these model names
   iOptions.getValues("models", mModels);
   //! Which dataset should data be selected from? (Default to main forecast dataset)
   iOptions.getValue("dataset", mDataset);
   iOptions.check();
}

void SelectorMember::selectCore(int iDate,
      int iInit,
      float iOffset,
      const Location& iLocation,
      const std::string& iVariable,
      const Parameters& iParameters,
      std::vector<Field>& iFields) const {

   Input* input;
   if(mDataset == "") {
      input = mData.getInput();
   }
   else {
      input = mData.getInput(mDataset);
   }
   std::vector<Member> members = input->getMembers();
   std::string datasetName = input->getName();

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

            Field slice(iDate, iInit, iOffset, members[id]);
            iFields.push_back(slice);
         }
      }
   }
}

int SelectorMember::getMaxMembers() const {
   return (int) mMembers.size();
}
