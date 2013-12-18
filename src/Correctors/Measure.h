#ifndef CORRECTOR_MEASURE_H
#define CORRECTOR_MEASURE_H
#include "Corrector.h"

class Measure;
//! Replace full ensemble with a single value created from an ensemble measure
class CorrectorMeasure : public Corrector {
   public:
      CorrectorMeasure(const Options& iOptions, const Data& iData);
      ~CorrectorMeasure();
      bool needsTraining() const {return false;};
   private:
      void correctCore(const Parameters& iParameters, Ensemble& iUnCorrected) const;
      Measure* mMeasure;
};
#endif
