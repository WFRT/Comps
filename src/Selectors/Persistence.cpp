#include "Persistence.h"
#include "../Field.h"
#include "../Data.h"

SelectorPersistence::SelectorPersistence(const Options& iOptions, const Data& iData) :
      Selector(iOptions, iData),
      mUseLatest(false),
      mUseObsAt(Global::MV) {
   //! Use today's obs at 00 UTC. Otherwise use yesterday's obs at the same time yesterday.
   iOptions.getValue("useLatest", mUseLatest);
   //! Use obs at the specified UTC time. Uses the last obs available before initialization.
   //! 'useLatest' and 'useObsAt' cannot both be specified.
   if(iOptions.getValue("useObsAt", mUseObsAt)) {
      if(mUseObsAt < 0 || mUseObsAt >= 24) {
         std::stringstream ss;
         ss << "useObsAt must be >= 0 and < 24";
         Global::logger->write(ss.str(), Logger::error);
      }
   }
   if(Global::isValid(mUseObsAt) && mUseLatest) {
      std::stringstream ss;
      ss << "Only one of 'useLatest' and 'useObsAt' can be used";
      Global::logger->write(ss.str(), Logger::error);
   }
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
         date = iDate;
         offset = iInit;
      }
      else if(Global::isValid(mUseObsAt)) {
         offset = mUseObsAt;
         if(init < mUseObsAt) {
            date = Global::getDate(iDate,init,-24);
         }
         else {
            date = iDate;
         }
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
