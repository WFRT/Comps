#include "Averager.h"
#include "Mean.h"
#include "../Distribution.h"

AveragerMean::AveragerMean(const Options& iOptions, const Data& iData) : Averager(iOptions, iData),
      mNumPoints(10) {
   //! How many sampling points should be used to compute mean of distribution?
   iOptions.getValue("numPoints", mNumPoints);
   iOptions.check();
}

float AveragerMean::average(const Distribution& iDist, const Parameters& iParameters) const {
   float mean = 0;
   // Mean of a distribution = integral x f(x) dx   on [-inf, inf]
   // However, its easier to sample the inverse, since then we can integrate from 0 to 1, instead of
   // having to figure out a good range of x-values to integrate over.
   // f(x) = dF(x)/dx
   // x = F^-1(P)
   // dx = dF / f(x)
   // mean = integral F^-1(P) dP    on [0,1]
   // Discretize using P = 1/(N+1) ... N/(N+1)
   for(int i = 0; i < mNumPoints; i++) {
      float P = float(i+1) / (mNumPoints + 1);
      float value = iDist.getInv(P);
      if(!Global::isValid(value))
         return Global::MV;
      mean += value;

   }
   return mean / mNumPoints;
}
