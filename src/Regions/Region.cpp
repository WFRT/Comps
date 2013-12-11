#include "Region.h"
#include "SchemesHeader.inc"
#include "../Global.h"
#include "../Options.h"
#include "../Data.h"
#include "../Location.h"

Region::Region(const Options& iOptions, const Data& iData) : Component(iOptions, iData),
      mEvenBins(false),
      mWindowLength(0),
      mLowerOffset(Global::MV),
      mUpperOffset(Global::MV) {
   //! Pool offsets by evenly spaced bins from the lowest to highest offset
   if(iOptions.getValue("evenBins", mEvenBins)) {
      //! Each bin should be this many hours wide
      iOptions.getRequiredValue("windowLength", mWindowLength);
   }
   //! These are passed down by the configuration
   iOptions.getRequiredValues("offsets", mOffsets);
   assert(mOffsets.size() > 0);
   mLowerOffset = mOffsets[0];
   mUpperOffset = mOffsets[mOffsets.size()-1];
   mLocationCache.setName("Region");
}
#include "Schemes.inc"

int Region::find(const Location& iLocation) const {
   if(mLocationCache.isCached(iLocation)) {
      // Use cached index
      int i = mLocationCache.get(iLocation)[0];
      return i;
   }
   else {
      // Compute and cache
      int i = findCore(iLocation);
      std::vector<int> temp(1,i);
      mLocationCache.add(iLocation, temp);
      return i;
   }
}
float Region::find(float iOffset) const {
   if(iOffset <= mLowerOffset) 
      return mLowerOffset;
   if(iOffset >= mUpperOffset) 
      return mUpperOffset;
   if(mEvenBins) {
      // Round to nearest bin edge
      // Is this right? 
      return round((float) (iOffset - mLowerOffset) / mWindowLength) * mWindowLength;
   }
   if(iOffset > mUpperOffset)
      return mUpperOffset;
   int   lower = Global::getLowerIndex(iOffset, mOffsets);
   int   upper = Global::getUpperIndex(iOffset, mOffsets);
   assert(Global::isValid(lower) && Global::isValid(upper));
   if(fabs(iOffset - mOffsets[lower]) > fabs(iOffset / mOffsets[upper]))
      return mOffsets[upper];
   else
      return mOffsets[lower];
   // Opportunity to cache index
}
