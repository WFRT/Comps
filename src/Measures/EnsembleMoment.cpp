#include "EnsembleMoment.h"
#include "../Options.h"
#include "../Ensemble.h"

MeasureEnsembleMoment::MeasureEnsembleMoment(const Options& iOptions, const Data& iData) : Measure(iOptions, iData) {
   iOptions.getRequiredValue("moment", mMoment);
   if(mMoment > 3 || mMoment < 1) {
      std::stringstream ss;
      ss << "MeasureEnsembleMoment: 'moment' must be 1 or 2";
      Global::logger->write(ss.str(), Logger::error);
   }
}

float MeasureEnsembleMoment::measureCore(const Ensemble& iEnsemble, const Parameters& iParameters) const {
   return iEnsemble.getMoment(mMoment);
}
bool MeasureEnsembleMoment::isPositiveCore() const {
   return (mMoment == 2);
}
