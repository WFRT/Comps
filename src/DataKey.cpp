#include "DataKey.h"

DataKey::DataKey(int rDate, int rInit, float iOffset, int rLocationId, std::string rVariableName, int rMemberId) :
   mDate(rDate),
   mInit(rInit),
   mOffset(iOffset),
   mLocationId(rLocationId),
   mVariableName(rVariableName),
   mMemberId(rMemberId)
{}
// TODO
bool DataKey::operator<(const DataKey &right) const {
   if(mOffset == right.getOffset()) {
      if(mDate == right.getDate()) {
         if(mMemberId ==right.getMemberId()) {
            if(mVariableName ==right.getVariableName()) {
               if(mLocationId == right.getLocationId()) {
                  return mInit < right.getInit();
               }
               else {
                  return mLocationId < right.getLocationId();
               }
            }
            else {
               return mVariableName < right.getVariableName();
            }
         }
         else {
            return mMemberId < right.getMemberId();
         }
      }
      else {
         return mDate < right.getDate();
      }
   }
   else {
      return mOffset < right.getOffset();
   }
}
int DataKey::getDate() const {
   return mDate;
}
int DataKey::getInit() const {
   return mInit;
}
int DataKey::getOffset() const {
   return mOffset;
}
int DataKey::getLocationId() const {
   return mLocationId;
}
std::string DataKey::getVariableName() const {
   return mVariableName;
}
int DataKey::getMemberId() const {
   return mMemberId;
//   return mMember->getId();
}
void DataKey::setDate(int rDate) {
   mDate = rDate;
}
void DataKey::setInit(int rInit) {
   mInit = rInit;
}
void DataKey::setOffset(float iOffset) {
   mOffset = iOffset;
}
void DataKey::setLocationId(int rLocationId) {
   mLocationId = rLocationId;
}
void DataKey::setVariableName(std::string rVariableName) {
   mVariableName = rVariableName;
}
void DataKey::setMemberId(int rMemberId) {
   mMemberId = rMemberId;
}
