#ifndef SLICE_H
#define SLICE_H
#include "Member.h"
class Slice {
   public:
      Slice(int iDate, int iInit, float iOffset, Member iMember, float iSkill = Global::MV);
      int getDate() const;
      int getInit() const;
      float getOffset() const;
      Member getMember() const;
      float getSkill() const;
      void setOffset(float iOffset);
   private:
      int mDate;
      int mInit;
      float mOffset;
      Member mMember;
      float mSkill;
};
#endif
