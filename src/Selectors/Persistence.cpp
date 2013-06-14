#include "Persistence.h"
#include "../Slice.h"
#include "../Data.h"

SelectorPersistence::SelectorPersistence(const Options& iOptions, const Data& iData) : Selector(iOptions, iData) {
   if(!iOptions.getValue("useLatest", mUseLatest)) {
      mUseLatest = false;
   }
}

void SelectorPersistence::selectCore(int iDate,
      int iInit,
      float iOffset,
      const Location& iLocation,
      const std::string& iVariable,
      const Parameters& iParameters,
      std::vector<Slice>& iSlices) const {

   int init = 0;
   float offset;
   int date;
   //Member member(mData.getObsInput()->getName());
   Member member(mData.getInput(iVariable,Input::typeObservation)->getName());
   if(mUseLatest) {
      // TODO
      offset = 0;
      date = iDate;
   }
   else {
      offset = fmod(iOffset,24);
      date = Global::getDate(iDate,init,-24);
   }
   Slice slice(date, init, offset, member); // TODO
   iSlices.push_back(slice);
}
