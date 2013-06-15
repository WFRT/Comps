#include "Selector.h"
#include "Extreme.h"
#include "../Data.h"
#include "../Slice.h"
#include "../Parameters.h"
#include "../Location.h"

SelectorExtreme::SelectorExtreme(const Options& iOptions, const Data& iData) : Selector(iOptions, iData) {}

void SelectorExtreme::selectCore(int iDate,
      int iInit,
      float iOffset,
      const Location& iLocation,
      const std::string& iVariable,
      const Parameters& iParameters,
      std::vector<Slice>& iSlices) const {

   std::vector<Member> members;
   mData.getMembers(iVariable, Input::typeForecast, members);
   for(int i = 0; i < (int) members.size(); i++) {
      Slice slice(iDate, iInit, iOffset, members[i]);
      iSlices.push_back(slice);
   }
}