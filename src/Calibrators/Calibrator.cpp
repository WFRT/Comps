#include "SchemesHeader.inc"
#include "Calibrator.h"
#include "../Options.h"
#include "../Distribution.h"
#include "../Parameters.h"
#include "../Variables/Variable.h"

Calibrator::Calibrator(const Options& iOptions, const Data& iData) :
      Processor(iOptions, iData),
      mInvTol(1e-4) {
   // When uncalibrating a value, how close must the cdf of that value be to the requested cdf?
   // Note this only applies when a Calibrator scheme does not implement its own inverter
   iOptions.getValue("invTol", mInvTol);
}
float Calibrator::unCalibrate(float iCdf, const Distribution::ptr iDist, const Parameters& iParameters) const {
   double s = Global::clock();
   float step = 0.1;
   float rawCdf = iCdf; // Starting value
   int maxIterations = 1000;
   int dir = 0;
   int counter = 0;

   float iP0 = 0;
   float iP1 = 0;
   const Variable* var = Variable::get(iDist->getVariable());
   if(var->isLowerDiscrete()) {
      iP0 = iDist->getCdf(var->getMin());
   }
   if(var->isLowerDiscrete()) {
      iP1 = iDist->getCdf(var->getMax());
   }

   //std::cout << "Iterative:" << iCdf << std::endl;
   float calCdf;
   while(counter < maxIterations) {
      calCdf = calibrate(rawCdf, iDist, iParameters);
      //std::cout << "  #" << dir << " " << step << " " << rawCdf << " " << calCdf << std::endl;
      if(!Global::isValid(calCdf)){
         return Global::MV;
      }

      if(fabs(calCdf - iCdf) < mInvTol) {
         //std::cout << "Withing tolerance" << std::endl;
         break;
      }
      // Continue going up
      if(calCdf < iCdf && dir == 1) {
         rawCdf = rawCdf + step;
      }
      // Continue going down
      else if(calCdf > iCdf && dir == -1) {
         rawCdf = rawCdf - step;
      }
      // Go up
      else if(calCdf < iCdf && dir != 1) {
         step = step / 2;
         dir = 1;
         rawCdf = rawCdf + step;
      }
      // Go down
      else if(calCdf > iCdf && dir != -1) {
         step = step / 2;
         dir = -1;
         rawCdf = rawCdf - step;
      }

      if(rawCdf <= 0) {
         rawCdf = mInvTol;
      }
      else if(rawCdf >= 1) {
         rawCdf = 1-mInvTol;
      }
      counter++;
   }
   //std::cout << "Counter: " << counter << " Desired cdf: " << iCdf << " Best raw cdf: " << rawCdf << " giving: " << calCdf << std::endl;
   double e = Global::clock();
   std::cout << "Calibrator::uncalibrate: " << e - s << std::endl;
   assert(rawCdf >= 0 && rawCdf <= 1);
   return rawCdf;

}
#include "Schemes.inc"

// TODO: Take into account P0 and P1
float Calibrator::amplify(float iCdf, const Distribution::ptr iDist, const Parameters& iParameters) const {
   // TODO:
   assert(0);
   return 1;
}

Distribution::ptr Calibrator::getDistribution(const Distribution::ptr iUpstream, Parameters iParameters) const {
   return Distribution::ptr(new DistributionCalibrator(iUpstream, *this, iParameters));
}
