#include "Updater.h"
#include "../Options.h"
#include "SchemesHeader.inc"

Updater::Updater(const Options& iOptions, const Data& iData) : Component(iOptions, iData) {
   /*
   Options opt;
   std::string estimatorTag;
   if!(iOptions.getValue("estimator", estimatorTag)) {
      Scheme::getOptions(estimatorTag, opt);
      mEstimator = EstimatorProabilistic::getScheme(opt);
   }
   else {
      mEstimator = NULL;
   }
   */
}
Updater::~Updater() {
   // delete estimator;
}

float Updater::amplify(float rCdf, int n, float rPit, const Parameters& rParameters) const {
   // TODO
   return 1;
}
#include "Schemes.inc"
void Updater::updateParameters(const Obs& iObs, float iCdf, int n, float iPit, Parameters& iParameters) const {
   // TODO
   //mEstimator->update(*this, iObs, iCdf, iPit, iParameters);
}

/*
Distribution::ptr Updater::getDistribution(const Distribution::ptr iUpstream, Parameters iParameters) const {
   return Distribution::ptr(new DistributionUpdater(iUpstream, *this, iParameters));
}
*/
