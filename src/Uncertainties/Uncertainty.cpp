#include "Uncertainty.h"
#include "SchemesHeader.inc"
#include "../Options.h"
#include "../Scheme.h"
#include "../Parameters.h"
#include "../Distribution.h"

Uncertainty::Uncertainty(const Options& iOptions, const Data& iData) : Processor(iOptions, iData) {

}

#include "Schemes.inc"

Distribution::ptr Uncertainty::getDistribution(Ensemble iEnsemble, const Averager& iAverager, const Parameters iParameters) const {
   return Distribution::ptr(new DistributionUncertainty(*this, iEnsemble, iAverager, iParameters));
}
