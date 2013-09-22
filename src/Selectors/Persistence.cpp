#include "Persistence.h"
#include "../Field.h"
#include "../Data.h"

SelectorPersistence::SelectorPersistence(const Options& iOptions, const Data& iData) :
      Selector(iOptions, iData),
      mUseLatest(false) {
   //! Use today's obs at 00 UTC. Otherwise use yesterday's obs at the same time
   iOptions.getValue("useLatest", mUseLatest);
}

void SelectorPersistence::selectCore(int iDate,
      int iInit,
      float iOffset,
      const Location& iLocation,
      const std::string& iVariable,
      const Parameters& iParameters,
      std::vector<Field>& iFields) const {

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
   Field slice(date, init, offset, member); // TODO
   iFields.push_back(slice);
}
