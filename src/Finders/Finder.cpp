#include "Finder.h"
#include "SchemesHeader.inc"
#include "../Global.h"
#include "../Options.h"
#include "../Data.h"

Finder::Finder(const Options& iOptions, const Data& iData) : Component(iOptions, iData),
      mSize(Global::MV) {

}
#include "Schemes.inc"

int Finder::find(const Location& iLocation) const {
   int i = findCore(iLocation);
   return i;
}
int Finder::size() const { 
   assert(mSize != Global::MV);
   return mSize;
}
