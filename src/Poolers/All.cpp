#include "All.h"
#include "../Global.h"
#include "../Options.h"
#include "../Location.h"

PoolerAll::PoolerAll(const Options& iOptions, const Data& iData) :
      Pooler(iOptions, iData) {
   iOptions.check();
}

int PoolerAll::findCore(const Location& iLocation) const {
   return 0;
}
