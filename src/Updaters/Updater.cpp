#include "SchemesHeader.inc"
#include "Updater.h"
#include "../Options.h"
#include "../Variables/Variable.h"

Updater::Updater(const Options& iOptions, const Data& iData) : Processor(iOptions, iData) {
   mType = Processor::TypeUpdater;
   /*
   Options opt;
   std::string estimatorTag;
   if!(iOptions.getValue("estimator", estimatorTag)) {
      mEstimator = EstimatorProabilistic::getScheme(estimatorTag);
   }
   else {
      mEstimator = NULL;
   }
   */
}
Updater::~Updater() {
   // delete estimator;
}

float Updater::unUpdate(float iCdf, const Obs& iRecentObs, const Distribution::ptr iRecent, const Distribution::ptr iDist, const Parameters& iParameters) const {

   float mInvTol = 1e-4;
   // TODO: Duplicate of Calibrator::unCalibrate
   float step = 0.1;
   float rawCdf = iCdf; // Starting value
   int maxIterations = 1000;
   int dir = 0;
   int counter = 0;

   // TODO: Any special treatment needed to deal with discrete-mixed variables?
   /*
   float iP0 = 0;
   float iP1 = 0;
   const Variable* var = Variable::get(iDist->getVariable());
   if(var->isLowerDiscrete()) {
      iP0 = iDist->getCdf(var->getMin());
   }
   if(var->isLowerDiscrete()) {
      iP1 = iDist->getCdf(var->getMax());
   }
   */

   float calCdf;
   while(counter < maxIterations) {
      calCdf = update(rawCdf, iRecentObs, iRecent, iDist, iParameters);
      if(!Global::isValid(calCdf)){
         return Global::MV;
      }

      if(fabs(calCdf - iCdf) < mInvTol) {
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
   assert(rawCdf >= 0 && rawCdf <= 1);
   return rawCdf;
}

float Updater::amplify(float iCdf, const Obs& iRecentObs, const Distribution::ptr iRecent, const Distribution::ptr iDist, const Parameters& rParameters) const {
   // TODO
   return 1;
}
#include "Schemes.inc"
void Updater::updateParameters(const std::vector<Distribution::ptr>& iDists,
      const std::vector<Obs>& iObs,
      const std::vector<Distribution::ptr>& iRecentDists,
      const std::vector<Obs>& iRecentObs,
      Parameters& iParameters) const {
   // TODO
   //mEstimator->update(*this, iObs, iCdf, iPit, iParameters);
}

Distribution::ptr Updater::getDistribution(const Distribution::ptr iUpstream, const Obs& iRecentObs, const Distribution::ptr iRecent, const Parameters& iParameters) const {
   return Distribution::ptr(new DistributionUpdater(iUpstream, *this, iRecentObs, iRecent, iParameters));
}
