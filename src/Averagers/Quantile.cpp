#include "Averager.h"
#include "Quantile.h"
#include "../Distribution.h"

AveragerQuantile::AveragerQuantile(const Options& iOptions, const Data& iData) : Averager(iOptions, iData) {
   iOptions.getRequiredValue("quantile", mQuantile);
}

float AveragerQuantile::average(const Distribution& iDist, const Parameters& iParameters) const {
   return iDist.getInv(mQuantile);
}
