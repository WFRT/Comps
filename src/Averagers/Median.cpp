#include "Averager.h"
#include "Median.h"

AveragerMedian::AveragerMedian(const Options& iOptions, const Data& iData) : Averager(iOptions, iData) {}

float AveragerMedian::average(const Ensemble& iValues, const Parameters& iParameters) const {
   float median;
   // Remove missing values
   std::vector<float> temp;
   for(int i = 0; i < iValues.size(); i++) {
      if(iValues[i] != Global::MV) {
         temp.push_back(iValues[i]);
      }
   }
   if(temp.size() == 0) {
      median = Global::MV;
   }
   else {
      std::sort(temp.begin(), temp.end());
      unsigned int N = temp.size();
      // Even size
      if(N % 2 == 0) {
         median = (temp[N/2 - 1] + temp[N/2])/2;
      }
      // Odd size
      else {
         median = temp[floor(N/2)];
      }
   }
   return median;
}
