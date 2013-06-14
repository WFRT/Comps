#include "Clim.h"
#include "../Global.h"
#include "../Options.h"

FinderClim::FinderClim(const Options& iOptions, const Data& iData) : Finder(iOptions, iData) {
   mSize = 10;
}
int FinderClim::findCore(const Location& iLocation) const {
   // Compute how similar their climatologies are
   return 0;
}
