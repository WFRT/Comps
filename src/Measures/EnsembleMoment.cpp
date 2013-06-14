#include "EnsembleMoment.h"
#include "../Options.h"
#include "../Ensemble.h"

MeasureEnsembleMoment::MeasureEnsembleMoment(const Options& iOptions, const Data& iData) : Measure(iOptions, iData) {
   iOptions.getRequiredValue("moment", mMoment);
}

float MeasureEnsembleMoment::measureCore(const Ensemble& iEnsemble, const Parameters& iParameters) const {
   return iEnsemble.getMoment(mMoment);
}
