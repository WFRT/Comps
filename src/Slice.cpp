#include "Slice.h"
Slice::Slice(int iDate, int iInit, float iOffset, Member iMember, float iSkill) :
   mDate(iDate),
   mInit(iInit),
   mOffset(iOffset),
   mMember(iMember),
   mSkill(iSkill) {}
int Slice::getDate() const {
   return mDate;
}
int Slice::getInit() const {
   return mInit;
}
float Slice::getOffset() const {
   return mOffset;
}
Member Slice::getMember() const {
   return mMember;
}
float Slice::getSkill() const {
   return mSkill;
}
void Slice::setOffset(float iOffset) {
   mOffset = iOffset;
}
