#include "Selector.h"
#include "Extreme.h"
#include "../Data.h"
#include "../Field.h"
#include "../Parameters.h"
#include "../Location.h"

SelectorExtreme::SelectorExtreme(const Options& iOptions, const Data& iData) : Selector(iOptions, iData) {}

void SelectorExtreme::selectCore(int iDate,
      int iInit,
      float iOffset,
      const Location& iLocation,
      const std::string& iVariable,
      const Parameters& iParameters,
      std::vector<Field>& iFields) const {

   std::vector<Member> members;
   mData.getMembers(iVariable, Input::typeForecast, members);
   for(int i = 0; i < (int) members.size(); i++) {
      Field slice(iDate, iInit, iOffset, members[i]);
      iFields.push_back(slice);
   }
}
