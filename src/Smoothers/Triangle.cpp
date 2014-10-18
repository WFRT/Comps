#include "Triangle.h"

SmootherTriangle::SmootherTriangle(const Options& iOptions, const Data& iData) : Smoother(iOptions, iData) {
   iOptions.getRequiredValue("width", mWidth);
   iOptions.check();
}
void SmootherTriangle::smooth(const std::vector<float>& iValues,
      const Parameters& iParameters,
      std::vector<float>& iSmoothedValues) const {
   iSmoothedValues.clear();

   int maxWeight = mWidth + 1;

   for(int i = 0; i < (int) iValues.size(); i++) {
      float value;
      if(iValues[i] != Global::MV) {
         // Set starting and ending points for filter
         int startT = i - mWidth;
         int endT   = i + mWidth;
         if(startT < 0)
            startT = 0;
         if(endT >= iValues.size())
            endT = iValues.size()-1;

         float totalValue = 0;
         float totalWeight = 0;
         for(int t = startT; t <= endT; t++) {
            if(iValues[t] != Global::MV) {
               float weight;
               weight = maxWeight - abs(t - i);
               assert(weight != 0);
               totalValue += weight*iValues[t];
               totalWeight += weight;
            }
         }
         if(totalWeight == 0) {
            // This should never occurr, since iValues[i] is guaranteed to be non-missing
            value = Global::MV;
         }
         else {
            value = totalValue / totalWeight;
         }
      }
      else {
         value = Global::MV;
      }
      iSmoothedValues.push_back(value);
   }
}
