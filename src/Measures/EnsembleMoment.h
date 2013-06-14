#ifndef MEASURE_ENSEMBLE_MOMENT_H
#define MEASURE_ENSEMBLE_MOMENT_H
#include "Measure.h"

//! Returns a specific moment of the ensemble
class MeasureEnsembleMoment : public Measure {
   public:
      MeasureEnsembleMoment(const Options& iOptions, const Data& iData);
   protected:
      float measureCore(const Ensemble& iEnsemble, const Parameters& iParameters) const;
      int mMoment;
};
#endif
