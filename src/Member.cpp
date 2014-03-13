#include "Member.h"
#include "Options.h"
#include "Namelist.h"
#include "Inputs/Input.h"

Member::Member(const std::string& iDataset, int iId) :
    mId(iId), mDataset(iDataset), mResolution(Global::MV) {}

Member::Member(const std::string& iDataset, float iResolution, std::string iModel, int iId) : mId(iId), mDataset(iDataset), mResolution(iResolution), mModel(iModel) {}

int Member::getId() const {
   return mId;
}
std::string Member::getDataset() const {
   return mDataset;
}
float Member::getResolution() const {
   return mResolution;
}
std::string Member::getModel() const {
   return mModel;
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
