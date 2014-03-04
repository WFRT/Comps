#include "BaseDistribution.h"
#include "SchemesHeader.inc"

BaseDistribution::BaseDistribution(const Options& iOptions, const Data& iData) : Component(iOptions),
      mData(iData) {
}

#include "Schemes.inc"
