#include "Key.h"

Key::DateVar::DateVar(int iDate, int iLocationId) : mDate(iDate), mLocationId(iLocationId) {}
bool Key::DateVar::operator<(const Key::DateVar &right) const {
   if(mDate ==right.mDate) {
      return mLocationId < right.mLocationId;
   }
   else {
      return mDate < right.mDate;
   }
}
float Key::DateVar::getSize() const {
   return 2*sizeof(int);
}

//----------------------------
Key::DateOffset::DateOffset(int iDate, float iOffset) : mDate(iDate), mOffset(iOffset) {}
bool Key::DateOffset::operator<(const Key::DateOffset &right) const {
   if(mDate ==right.mDate) {
      return mOffset < right.mOffset;
   }
   else {
      return mDate < right.mDate;
   }
}
float Key::DateOffset::getSize() const {
   return sizeof(int) + sizeof(float);
}

//----------------------------
Key::DateLoc::DateLoc(int iDate, int iLocationId) : mDate(iDate), mLocationId(iLocationId) {}
bool Key::DateLoc::operator<(const DateLoc &right) const {
   if(mDate ==right.mDate) {
      return mLocationId < right.mLocationId;
   }
   else {
      return mDate < right.mDate;
   }
}
float Key::DateLoc::getSize() const {
   return 2*sizeof(int);
}

//----------------------------
Key::DateLocVar::DateLocVar(int iDate, int iLocationId, std::string iVariableId) : mDate(iDate), mLocationId(iLocationId), mVariableId(iVariableId) {}
bool Key::DateLocVar::operator<(const DateLocVar &right) const {
   if(mDate ==right.mDate) {
      if(mLocationId == right.mLocationId) {
         return mVariableId < right.mVariableId;
      }
      else {
         return mLocationId < right.mLocationId;
      }
   }
   else {
      return mDate < right.mDate;
   }
}
float Key::DateLocVar::getSize() const {
   return 2*sizeof(int) + mVariableId.size() * sizeof(char);
}

//----------------------------
Key::Input::Input(int iDate, int iInit, float iOffset, int iLocationId, int iMemberId, int iVariableId) :
      date(iDate), init(iInit), offset(iOffset), location(iLocationId),
      member(iMemberId), variable(iVariableId) {}
bool Key::Input::operator<(const Input &right) const {
   if(date == right.date) {
      if(init == right.init) {
         if(offset == right.offset) {
            if(location == right.location) {
               if(member == right.member) {
                  return variable < right.variable;
               }
               return member < right.member;
            }
            return location < right.location;
         }
         return offset < right.offset;
      }
      return init < right.init;
   }
   return date < right.date;
}
bool Key::Input::operator==(const Input& right) const {
   return date == right.date &&
      init == right.init &&
      offset == right.offset &&
      location == right.location &&
      member == right.member &&
      variable == right.variable;
}
float Key::Input::getSize() const {
   return 5*sizeof(int) + sizeof(float);
}

namespace Key {
   std::ostream& operator<<(std::ostream &os, const Key::Input& i) {
      os << i.date << " " << i.init << " " << i.offset << " " << i.location << " " << i.member << " " << i.variable;
      return os;
   }
}

//-------------

Key::Ensemble::Ensemble(int iDate, int iInit, float iOffset, int iLocationId, std::string iVariable) :
   date(iDate), init(iInit), offset(iOffset), location(iLocationId), variable(iVariable) {}

bool Key::Ensemble::operator<(const Key::Ensemble &right) const {
   if(date == right.date) {
      if(init == right.init) {
         if(offset == right.offset) {
            if(location == right.location) {
               return variable < right.variable;
            }
            return location < right.location;
         }
         return offset < right.offset;
      }
      return init < right.init;
   }
   return date < right.date;
}
float Key::Ensemble::getSize() const {
   return 3*sizeof(int) + 1*sizeof(float) + variable.size() * sizeof(char);
}

//---------------
Key::DateOffsetLocVar::DateOffsetLocVar(int iDate, int iOffset, int iLocationId, std::string iVariableId) : mDate(iDate), mOffset(iOffset), mLocationId(iLocationId), mVariableId(iVariableId) {}
bool Key::DateOffsetLocVar::operator<(const DateOffsetLocVar &right) const {
   if(mDate ==right.mDate) {
      if(mOffset == right.mOffset) {
         if(mLocationId == right.mLocationId) {
            return mVariableId < right.mVariableId;
         }
         else {
            return mLocationId < right.mLocationId;
         }
      }
      else {
         return mOffset < right.mOffset;
      }
   }
   else {
      return mDate < right.mDate;
   }
}
float Key::DateOffsetLocVar::getSize() const {
   return 3*sizeof(int) + mVariableId.size() * sizeof(char);
}

//---------------
Key::DateInitVarConfig::DateInitVarConfig(int iDate, int iInit, std::string iVariable, std::string iConfigurationName) : mDate(iDate), mInit(iInit), mVariable(iVariable), mConfigurationName(iConfigurationName) {}

bool Key::DateInitVarConfig::operator<(const Key::DateInitVarConfig& right) const {
   if(mDate ==right.mDate) {
      if(mInit ==right.mInit) {
         if(mVariable == right.mVariable) {
            return mConfigurationName < right.mConfigurationName;
         }
         else {
            return mVariable < right.mVariable;
         }
      }
      else {
         return mInit < right.mInit;
      }
   }
   else {
      return mDate < right.mDate;
   }
}

float Key::DateInitVarConfig::getSize() const {
   return 2*sizeof(int) + (mVariable.size() + mConfigurationName.size()) * sizeof(char);
}

//---------------------------
Key::Par::Par(Component::Type iType,
                  int iDate,
                  int iInit,
                  float iOffset,
                  int iLocationId,
                  std::string iVariable,
                  std::string iConfigurationName,
                  int iIndex) :
      mType(iType),
      mOffset(iOffset),
      mDate(iDate),
      mInit(iInit),
      mLocationId(iLocationId),
      mVariable(iVariable),
      mConfigurationName(iConfigurationName),
      mIndex(iIndex) {
}

bool Key::Par::operator<(const Key::Par &right) const {
   if(mType == right.mType) {
      if(mDate == right.mDate) {
         if(mInit == right.mInit) {
            if(mOffset == right.mOffset) {
               if(mLocationId == right.mLocationId) {
                  if(mVariable == right.mVariable) {
                     if(mIndex == right.mIndex) {
                        return mConfigurationName < right.mConfigurationName;
                     }
                     else {
                        return mIndex < right.mIndex;
                     }
                  }
                  return mVariable < right.mVariable;
               }
               return mLocationId < right.mLocationId;
            }
            return mOffset < right.mOffset;
         }
         return mInit < right.mInit;
      }
      return mDate < right.mDate;
   }
   return mType < right.mType;
}
bool Key::Par::operator==(const Par& right) const {
   return (mType == right.mType) && (mDate == right.mDate) && (mInit == right.mInit) && (mOffset == right.mOffset) && (mLocationId == right.mLocationId) && (mVariable == right.mVariable) && (mConfigurationName == right.mConfigurationName) && (mIndex == right.mIndex);
}
float Key::Par::getSize() const {
   return 6*sizeof(int) + (mVariable.size() + mConfigurationName.size()) * sizeof(char);
}

std::string Key::Par::toString() const {
   std::stringstream ss;
   ss <<
      mType << " : " << 
      mDate << " : " <<
      mInit << " : " <<
      mOffset << " : " <<
      mLocationId << " : " <<
      mVariable << " : " <<
      mConfigurationName << " :" <<
      mIndex << " :";
   return ss.str();
}
