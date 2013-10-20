#ifndef MEASURE_FRACTION_H
#define MEASURE_FRACTION_H
#include "Measure.h"

//! Returns the fraction of members equal to some preset threshold
class MeasureFraction : public Measure {
   public:
      MeasureFraction(const Options& iOptions, const Data& iData);
   protected:
      float measureCore(const Ensemble& iEnsemble) const;
      float mX;
      bool mUseMin;
      bool mUseMax;
};
#endif
