#include "Member.h"
#include "Options.h"
#include "Namelist.h"
#include "Inputs/Input.h"

Member::Member(const std::string& iDataset, int iId) :
    mId(iId), mDataset(iDataset), mIsCached(false), mResolution(Global::MV) {}

Member::Member(const std::string& iDataset, float iResolution, std::string iModel, int iId) : mId(iId), mDataset(iDataset), mResolution(iResolution), mModel(iModel), mIsCached(true) {}

int Member::getId() const {
   return mId;
}
std::string Member::getDataset() const {
   return mDataset;
}
float Member::getResolution() const {
   if(!mIsCached) {
      loadValues();
   }
   return mResolution;
}
std::string Member::getModel() const {
   if(!mIsCached) {
      loadValues();
   }
   return mModel;
}
void Member::loadValues() const {
   std::stringstream ss0;
   ss0 << Input::getInputDirectory() << mDataset << "/members.nl";
   Namelist nl(ss0.str());
   std::stringstream ss;
   ss << mId;
   Options opt;
   if(nl.getOptions(ss.str(), opt)) {
      if(!opt.getValue("model", mModel)) {
         mModel = "";
      }
      if(!opt.getValue("resolution", mResolution)) {
         mResolution = Global::MV;
      }
   }
   mIsCached = true;
}

bool Member::operator<(const Member &right) const {
   if(mId == right.mId) {
      return mDataset < right.mDataset;
   }
   else {
      return mId < right.mId;
   }
}

float Member::getSize() const {
   return sizeof(int) + mDataset.size()*sizeof(char);
}
