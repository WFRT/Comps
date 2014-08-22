#include "Persistence.h"
#include "../Field.h"
#include "../Data.h"

SelectorPersistence::SelectorPersistence(const Options& iOptions, const Data& iData) :
      Selector(iOptions, iData),
      mUseLatest(false) {
   //! Use today's obs at 00 UTC. Otherwise use yesterday's obs at the same time
   iOptions.getValue("useLatest", mUseLatest);
   iOptions.check();
}

void SelectorPersistence::selectCore(int iDate,
      int iInit,
      float iOffset,
      const Location& iLocation,
      const std::string& iVariable,
      const Parameters& iParameters,
      std::vector<Field>& iFields) const {

   Input* input = mData.getInput(iVariable,Input::typeObservation);
   if(input != NULL) {
      int date;
      int init = iInit;
      float offset;
      Member member(input->getName());
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
   else {
      std::stringstream ss;
      ss << "SelectorPersistence: No observation dataset available for variable '"
         << iVariable << "'";
      Global::logger->write(ss.str(), Logger::warning);
   }
}
