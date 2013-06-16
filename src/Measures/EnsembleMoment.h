#ifndef MEASURE_ENSEMBLE_MOMENT_H
#define MEASURE_ENSEMBLE_MOMENT_H
#include "Measure.h"

//! Returns a specific moment of the ensemble
class MeasureEnsembleMoment : public Measure {
   public:
      //! Options:
      //! * moment: Which ensemble moment should we use (1 or 2)?
      MeasureEnsembleMoment(const Options& iOptions, const Data& iData);
   protected:
      float measureCore(const Ensemble& iEnsemble, const Parameters& iParameters) const;
      bool  isPositiveCore() const;
      int mMoment;
};
#endif
