#ifndef KEY_H
#define KEY_H
#include "Component.h"
#include <iostream>
#include <ostream>

namespace Key {
   class DateVar {
      public:
         DateVar(int iDate, int iLocationId);
         bool operator<(const DateVar &right) const;
         int mDate;
         int mLocationId;
         float getSize() const;
   };
   class DateOffset {
      public:
         DateOffset(int iDate, float iOffset);
         bool operator<(const DateOffset &right) const;
         int mDate;
         float mOffset;
         float getSize() const;
   };
   class DateLoc {
      public:
         DateLoc(int iDate, int iLocationId);
         bool operator<(const DateLoc &right) const;
         int mDate;
         int mLocationId;
         float getSize() const;
   };
   class DateLocVar {
      public:
         DateLocVar(int iDate, int iLocationId, std::string iVariableId);
         bool operator<(const DateLocVar &right) const;
         int mDate;
         int mLocationId;
         std::string mVariableId;
         float getSize() const;
   };
   class Input {
      public:
         Input(int iDate=Global::MV, int iInit=Global::MV, float iOffset=Global::MV, int iLocationId=Global::MV, int iMemberId=Global::MV, int iVariableId=Global::MV);
         bool operator<(const Input &right) const;
         bool operator==(const Input& right) const;
         friend std::ostream& operator<<(std::ostream &os, const Key::Input& i);
         int date;
         int init;
         float offset;
         int location;
         int member;
         int variable;
         float getSize() const;
   };
   class Ensemble {
      public:
         Ensemble(int iDate, int iInit, float iOffset, int iLocationId, std::string iVariable);
         bool operator<(const Ensemble &right) const;
         int date;
         int init;
         float offset;
         int location;
         std::string variable;
         float getSize() const;
   };

   template <class K, class V, class T>
   class Three {
      public:
         Three(K i1, V i2, T i3) : m1(i1), m2(i2), m3(i3) {};
         bool operator<(const Three &right) const {
            if(m1 == right.m1) {
               if(m2 == right.m2) {
                  return m3 < right.m3;
               }
               else {
                  return m2 < right.m2;
               }
            }
            else {
               return m1 < right.m1;
            }
         };
         K m1;
         V m2;
         T m3;
         float getSize() const {
            return sizeof(K) + sizeof(V) + sizeof(T);
         };
   };
   class DateOffsetLocVar {
      public:
         DateOffsetLocVar(int iDate, int iOffset, int iLocationId, std::string iVariableId);
         bool operator<(const DateOffsetLocVar &right) const;
         int mDate;
         int mOffset;
         int mLocationId;
         std::string mVariableId;
         float getSize() const;
   };
   class DateInitVarConfig {
      public:
         DateInitVarConfig(int iDate, int iInit, std::string iVariable, std::string iConfigurationName);
         int mDate;
         int mInit;
         std::string mVariable;
         std::string mConfigurationName;
         bool operator<(const DateInitVarConfig& right) const;
         float getSize() const;
   };
   class Par {
      public:
         Par(Component::Type iType,
               int iDate,
               int iInit,
               float iOffset,
               int iLocationId,
               std::string iVariable,
               std::string iConfigurationName,
               int iIndex);
         Component::Type mType;
         int mOffset;
         int mDate;
         int mInit;
         int mLocationId;
         std::string mVariable;
         std::string mConfigurationName;
         int mIndex;
         bool operator<(const Par& right) const;
         bool operator==(const Par& right) const;
         float getSize() const;

         std::string toString() const;
   };
};
#endif

