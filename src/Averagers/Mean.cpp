#include "Averager.h"
#include "Mean.h"

AveragerMean::AveragerMean(const Options& iOptions, const Data& iData) : Averager(iOptions, iData) {}

float AveragerMean::average(const Ensemble& iValues, const Parameters& iParameters) const {
   float mean = 0;
   int counter = 0;
   for(int i = 0; i < (int) iValues.size(); i++) {
      if(iValues[i] != Global::MV) {
         mean += iValues[i];
         counter++;
      }
   }
   if(counter != 0) {
      mean = mean / counter;
   }
   else  {
      mean = Global::MV;
   }
   return mean;
}
