#include "Anomaly.h"
#include "../Data.h"
#include "../Ensemble.h"

MeasureAnomaly::MeasureAnomaly(const Options& iOptions, const Data& iData) : Measure(iOptions, iData) {
   iOptions.check();
}

float MeasureAnomaly::measureCore(const Ensemble& iEnsemble) const {
   float clim = mData.getClim(iEnsemble.getDate(), 0, iEnsemble.getOffset(), iEnsemble.getLocation(), iEnsemble.getVariable());
   float mean = iEnsemble.getMoment(1);
   return mean - clim;
}
