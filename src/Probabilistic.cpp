#include "Probabilistic.h"
#include "Scheme.h"

Probabilistic::Probabilistic(const Options& iOptions, const Data& iData) : Processor(iOptions, iData) {
   // Set up estimator
   std::string estimatorTag;
   if(iOptions.getValue("estimator", estimatorTag)) {
      mEstimator = EstimatorProbabilistic::getScheme(estimatorTag, iData, *this);
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
