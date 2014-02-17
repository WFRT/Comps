#include "SchemesHeader.inc"
#include "Selector.h"
#include "../Inputs/Input.h"
#include "../Data.h"
#include "../Global.h"
#include "../Options.h"
#include "../Parameters.h"
#include "../Location.h"
#include "../Field.h"
#include "../Data.h"

Selector::Selector(const Options& iOptions, const Data& iData) :
      Processor(iOptions, iData), mRemoveMissing(false) {
   //! Removes ensemble members that value missing forecasts
   iOptions.getValue("removeMissing", mRemoveMissing);
}
#include "Schemes.inc"

void Selector::select(int iDate,
      int iInit,
      float iOffset,
      const Location& iLocation,
      const std::string& iVariable,
      const Parameters& iParameters,
      std::vector<Field>& iFields) const {
   selectCore(iDate, iInit, iOffset, iLocation, iVariable, iParameters, iFields);
   if(mRemoveMissing) {
      std::vector<Field>::iterator it = iFields.begin();
      while(it !=  iFields.end()) {
         std::cout << "Checking: " << it->getMember().getId() << std::endl;
         float value = mData.getValue(iDate, iInit, iOffset, iLocation, it->getMember(), iVariable);
         if(!Global::isValid(value)) {
            it = iFields.erase(it);
         }
         else {
            it++;
         }
      }
   }
}

int Selector::getMaxMembers() const {
   return mDefaultMaxMembers;
}

bool Selector::isConstSize() const {
   return isConstSizeCore() && !mRemoveMissing;
}
