#include "Uncertainty.h"
#include "SchemesHeader.inc"
#include "../Options.h"
#include "../Scheme.h"
#include "../Parameters.h"
#include "../Distribution.h"

Uncertainty::Uncertainty(const Options& iOptions, const Data& iData) : Processor(iOptions, iData) {

}

#include "Schemes.inc"

Distribution::ptr Uncertainty::getDistribution(Ensemble iEnsemble, Parameters iParameters, const Averager& iAverager, Parameters iAveragerParameters) const {
   return Distribution::ptr(new DistributionUncertainty(*this, iParameters, iEnsemble, iAverager, iAveragerParameters));
}
