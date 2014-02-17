#include "PitBased.h"
#include <boost/math/distributions/normal.hpp>
#include "../Parameters.h"
#include "../Obs.h"

UpdaterPitBased::UpdaterPitBased(const Options& iOptions, const Data& iData) : Updater(iOptions, iData) {
}
float UpdaterPitBased::update(float iCdf,
      const Obs& iRecentObs,
      const Distribution::ptr iRecent,
      const Distribution::ptr iDist,
      const Parameters& iParameters) const {
   float obs = iRecentObs.getValue();
   if(!Global::isValid(obs))
      return iCdf;

   float recentPit = iRecent->getCdf(obs);
   if(!Global::isValid(recentPit))
      return iCdf;

   float sigma0 = iParameters[0];
   float sigma = getSigma(sigma0);
   float returnValue = Global::MV;
   int   n = Global::getTimeDiff(iDist->getDate(), iDist->getInit(), iDist->getOffset(),
                                 iRecentObs.getDate(), iRecentObs.getInit(), iRecentObs.getOffset());

   // TODO
   if(n == 0) {
      if(iCdf < recentPit)
         return 0;
      else if(iCdf > recentPit)
         return 1;
      else
         return recentPit;
   }
   n = 1;

   if(sigma > 0.5) {
      return iCdf;
   }
   else {
      float accum = 0;
      for(int i = -mNumIterations; i <= mNumIterations; i++) {
         float mean = recentPit;
         float std  = sqrt((float) n)*sigma;
         assert(std > 0);
         boost::math::normal dist(mean, std);
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
         //assert(0);
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
   float sigma0 = 0.30;
   iParameters[0] = sigma0;
}

void UpdaterPitBased::updateParameters(const std::vector<Distribution::ptr>& iDists,
      const std::vector<Obs>& iObs,
      const std::vector<Distribution::ptr>& iRecentDists,
      const std::vector<Obs>& iRecentObs,
      Parameters& iParameters) const {
   // Own 
   assert(iDists.size() == iObs.size());
   assert(iDists.size() == iRecentDists.size());
   assert(iDists.size() == iRecentObs.size());
   float prevSigma = iParameters[0];
   int counter = 0;
   float total = 0;
   for(int i = 0; i < iDists.size(); i++) {
      int   n = Global::getTimeDiff(iDists[i]->getDate(), iDists[i]->getInit(), iDists[i]->getOffset(),
            iRecentObs[i].getDate(), 0, iRecentObs[i].getOffset());
      //std::cout << iDists[i]->getDate() << " " << iRecentObs[i].getDate() << " " << n << std::endl;
      if(n > 0) {
         //std::cout << "Update parameters: " << iObs[i].getValue() << " " << iRecentObs[i].getValue() << " " << n << std::endl;
         float recentObs = iRecentObs[i].getValue();
         float obs       = iObs[i].getValue();
         if(Global::isValid(recentObs) && Global::isValid(obs)) {
            float recentPit = iRecentDists[i]->getCdf(recentObs);
            float pit       = iDists[i]->getCdf(obs);
            if(Global::isValid(recentPit) && Global::isValid(pit)) {
               // TODO: Use the value of 'n'
               total += fabs(recentPit - pit);
               counter++;
            }
         }
      }
   }
   if(counter > 0) {
      float newSigma = total / counter;
      iParameters[0] = Processor::combine(prevSigma, newSigma, counter);
   }
}
