#ifndef MEMBER_H
#define MEMBER_H
#include "Global.h"

//! Represents an ensemble member in a dataset
class Member {
   public:
      Member(const std::string& iDataset="", int iId=0);
      Member(const std::string& iDataset, float iResolution, std::string iModel, int iId=0);
      int         getId() const;
      std::string getDataset() const;
      float       getResolution() const;
      std::string getModel() const;
      bool operator<(const Member &right) const;
      float getSize() const;
   private:
      std::string mDataset;
      int         mId;
      std::string mModel;
      float  mResolution;
};

#endif

