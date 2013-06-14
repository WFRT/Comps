#include "Probabilistic.h"
#include "Scheme.h"

Probabilistic::Probabilistic(const Options& iOptions, const Data& iData) : Component(iOptions, iData) {
   // Set up estimator
   std::string estimatorTag;
   if(iOptions.getValue("estimator", estimatorTag)) {
      Options opt;
      Scheme::getOptions(estimatorTag, opt);
      mEstimator = EstimatorProbabilistic::getScheme(opt, iData, *this);
   }
   else {
      mEstimator = NULL;
   }

}
Probabilistic::~Probabilistic() {
   if(mEstimator) {
      delete mEstimator;
   }
}
