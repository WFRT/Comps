#ifndef MEASURE_LOCAL_GRADIENT_H
#define MEASURE_LOCAL_GRADIENT_H
#include "Measure.h"

//! Returns the absolute value of the gradient at the location of the ensemble
class MeasureLocalGradient : public Measure {
   public:
      MeasureLocalGradient(const Options& iOptions, const Data& iData);
   protected:
      float measureCore(const Ensemble& iEnsemble, const Parameters& iParameters) const;
      std::string mVariable;
};
#endif
