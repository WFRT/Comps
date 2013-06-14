#ifndef MEASURE_ANOMALY_H
#define MEASURE_ANOMALY_H
#include "Measure.h"

//! Returns the deviation of the enemble mean from the climatological value
class MeasureAnomaly : public Measure {
   public:
      MeasureAnomaly(const Options& iOptions, const Data& iData);
   protected:
      //! Positive value means the ens mean is larger than normal
      float measureCore(const Ensemble& iEnsemble, const Parameters& iParameters) const;
};
#endif
