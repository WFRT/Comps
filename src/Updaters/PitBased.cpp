#include "PitBased.h"
#include <boost/math/distributions/normal.hpp>
#include "../Parameters.h"

UpdaterPitBased::UpdaterPitBased(const Options& iOptions, const Data& iData) : Updater(iOptions, iData) {
}
float UpdaterPitBased::update(float iCdf, int n, float iPit, const Parameters& iParameters) const {
   float sigma0 = iParameters[0];
   float sigma = getSigma(sigma0);
   float returnValue = Global::MV;
   if(sigma > 0.5) {
      returnValue = iCdf;
   }
   else {
      float accum = 0;
      for(int i = -mNumIterations; i <= mNumIterations; i++) {
         boost::math::normal dist(iPit, sqrt((float) n)*sigma);
         float part1 = boost::math::cdf(dist, iCdf + 2*i) - boost::math::cdf(dist, 2*i);
         float part2 = (1 - boost::math::cdf(dist, -iCdf + 2*i)) - (1 - boost::math::cdf(dist, 2*i));
         float diff = part1 + part2;
         if(diff < 0) diff = 0;
         if(diff > 1) diff = 1;
         accum += diff;
      }
      if(accum < 0) {
         accum = 0;
         assert(0);
      }
      else if(accum > 1) {
         accum = 1;
         assert(0);
      }
      returnValue = accum;
   }

   return returnValue;
}
float UpdaterPitBased::getSigma(float iSigma0) const {
   float sigma;
   if(iSigma0 > 0.3) {
      sigma = 10;
   }
   else {
      sigma = tan(iSigma0*3.5)/3.5;
   }
   return sigma;
}

void UpdaterPitBased::getDefaultParameters(Parameters& iParameters) const {
   float sigma0 = 0.10;
   iParameters[0] = sigma0;
}

void UpdaterPitBased::updateParameters(const Obs& iObs, float iCdf, int n, float iPit, Parameters& iParameters) const {
   // Own 

}
