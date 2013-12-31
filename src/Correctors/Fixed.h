#ifndef CORRECTOR_FIXED_H
#define CORRECTOR_FIXED_H
#include "Corrector.h"
#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>

//! Set all ensemble members to a fixed value
class CorrectorFixed : public Corrector {
   public:
      CorrectorFixed(const Options& iOptions, const Data& iData);
      bool needsTraining() const {return false;};
   private:
      void correctCore(const Parameters& iParameters, Ensemble& iUnCorrected) const;
      float mValue;
};
#endif
