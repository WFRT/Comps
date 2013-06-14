#ifndef MEMBER_H
#define MEMBER_H
#include "Global.h"

/** Represents an ensemble member */
class Member {
   public:
      Member(const std::string& rDataset="", int rId=0);
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
      // Caching
      void    loadValues() const;
      mutable std::string mModel;
      mutable float  mResolution;
      mutable bool   mIsCached;
};

#endif

