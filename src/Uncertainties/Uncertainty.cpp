#include "Uncertainty.h"
#include "SchemesHeader.inc"
#include "../Options.h"
#include "../Scheme.h"
#include "../Parameters.h"
#include "../Distribution.h"

Uncertainty::Uncertainty(const Options& iOptions, const Data& iData) : Component(iOptions, iData) {

}

#include "Schemes.inc"

Distribution::ptr Uncertainty::getDistribution(Ensemble iEnsemble, const Parameters iParameters) const {
   return Distribution::ptr(new DistributionUncertainty(*this, iEnsemble, iParameters));
}
