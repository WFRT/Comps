#include "Default.h"
#include "../Global.h"
#include "../Options.h"
#include "../Location.h"
#include "../Data.h"

FinderDefault::FinderDefault(const Options& iOptions, const Data& iData) : Finder(iOptions, iData) {
   iData.getOutputLocations(mLocations);
   mSize = (int) mLocations.size();
}

int FinderDefault::findCore(const Location& iLocation) const {
   return iLocation.getId();
}
