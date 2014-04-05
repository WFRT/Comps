#ifndef CORRECTOR_K_MEANS_H
#define CORRECTOR_K_MEANS_H
#include "Corrector.h"

//! Creates an ensemble with 'K' members, using the Kmeans algorithm
//! Means that have no closest ensemble members are set to missing
class CorrectorKmeans : public Corrector {
   public:
      CorrectorKmeans(const Options& iOptions, const Data& iData);
   private:
      void correctCore(const Parameters& iParameters, Ensemble& iUnCorrected) const;
      bool needsTraining() const {return false;};
      int mNumMeans;
      static float mTol;
};
#endif
