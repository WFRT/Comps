#include "Norm.h"
#include "../Data.h"

DetMetricNorm::DetMetricNorm(const Options& rOptions, const Data& iData) : DetMetric(rOptions, iData) {
   rOptions.getRequiredValue("order", mOrder);
   assert(mOrder > 0);
}

float DetMetricNorm::computeCore(const std::vector<std::pair<std::string, float> > & iData0,
      const std::vector<std::pair<std::string, float> >& iData1,
      const Parameters& iParameters,
      const Data& iData) const {
   int N = (int) iData1.size();
   float value = 0;
   int totalWeight = 0;
   // Loop over variables
   for(int i = 0; i < N; i++) {
      if(iData0[i].second != Global::MV && iData1[i].second != Global::MV) {
         totalWeight++;
         const Variable* var = Variable::get(iData0[i].first);
         if(var->isCircular()) {
            float lowerValue;
            float upperValue;
            if(iData0[i].second > iData1[i].second) {
               lowerValue = iData1[i].second;
               upperValue = iData0[i].second;
            }
            else {
               lowerValue = iData0[i].second;
               upperValue = iData1[i].second;
            }
            assert(upperValue >= lowerValue);
            float valueRight = pow(upperValue - lowerValue, mOrder);
            float valueLeft  = pow((var->getMax() - upperValue) + (lowerValue - var->getMin()), mOrder);
            value += std::min(valueLeft, valueRight);
         }
         else {
            value += pow(fabs(iData0[i].second - iData1[i].second), mOrder);
         }
      }
      else {
         // Skip this variable if it is missing for the target
      }
   }
   if(totalWeight == 0) {
      // All variables missing for target
      value = Global::INF;
   }
   else {
      value /= totalWeight;
   }
   return value;
}
