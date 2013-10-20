#ifndef MEASURE_ENSEMBLE_MEDIAN_H
#define MEASURE_ENSEMBLE_MEDIAN_H
#include "Measure.h"

//! Returns the median of the ensemble
class MeasureEnsembleMedian : public Measure {
   public:
      MeasureEnsembleMedian(const Options& iOptions, const Data& iData);
   protected:
      float measureCore(const Ensemble& iEnsemble) const;
};
#endif
