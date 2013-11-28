#include "Selector.h"
#include "Default.h"
#include "../Data.h"
#include "../Field.h"
#include "../Parameters.h"
#include "../Location.h"

SelectorDefault::SelectorDefault(const Options& iOptions, const Data& iData) : Selector(iOptions, iData), 
      mWindowLength(0) {
   iOptions.getValue("windowLength", mWindowLength);
}

void SelectorDefault::selectCore(int iDate,
      int iInit,
      float iOffset,
      const Location& iLocation,
      const std::string& iVariable,
      const Parameters& iParameters,
      std::vector<Field>& iFields) const {

   std::vector<Member> members;
   mData.getMembers(iVariable, Input::typeForecast, members);
   for(int i = 0; i < (int) members.size(); i++) {
      if(mWindowLength > 0) {
         for(float offset = iOffset - mWindowLength;
               offset <= iOffset + mWindowLength; offset++) {
            if(offset >= 0) {
               Field field(iDate, iInit, offset, members[i]);
               iFields.push_back(field);
            }
         }
      }
      else {
         Field field(iDate, iInit, iOffset, members[i]);
         iFields.push_back(field);
      }
   }
}
