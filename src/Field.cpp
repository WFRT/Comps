#include "Field.h"
Field::Field(int iDate, int iInit, float iOffset, Member iMember, float iSkill) :
   mDate(iDate),
   mInit(iInit),
   mOffset(iOffset),
   mMember(iMember),
   mSkill(iSkill) {}
int Field::getDate() const {
   return mDate;
}
int Field::getInit() const {
   return mInit;
}
float Field::getOffset() const {
   return mOffset;
}
Member Field::getMember() const {
   return mMember;
}
float Field::getSkill() const {
   return mSkill;
}
void Field::setOffset(float iOffset) {
   mOffset = iOffset;
}
