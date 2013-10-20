#include "EnsembleMoment.h"
#include "../Options.h"
#include "../Ensemble.h"

MeasureEnsembleMoment::MeasureEnsembleMoment(const Options& iOptions, const Data& iData) : Measure(iOptions, iData) {
   //! Which moment of the ensemble (between 0 and 2)? I.e. E[x^moment]. 2nd moment is the central
   //! moment.
   iOptions.getRequiredValue("moment", mMoment);
   if(mMoment > 2 || mMoment < 0) {
      std::stringstream ss;
      ss << "MeasureEnsembleMoment: 'moment' must be 0, 1 or 2";
      Global::logger->write(ss.str(), Logger::error);
   }
}

float MeasureEnsembleMoment::measureCore(const Ensemble& iEnsemble) const {
   if(mMoment == 0)
      return 1;
   else
      return iEnsemble.getMoment(mMoment);
}
bool MeasureEnsembleMoment::isPositiveCore() const {
   return (mMoment == 0 || mMoment == 2);
}
