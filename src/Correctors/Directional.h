#ifndef CORRECTOR_DIRECTIONAL_H
#define CORRECTOR_DIRECTIONAL_H
#include "Corrector.h"

//! Bias-correction based no wind-direction. Bias is computed for different wind direction.
class CorrectorDirectional : public Corrector {
   public:
      CorrectorDirectional(const Options& iOptions, const Data& iData);
   private:
      void correctCore(const Parameters& iParameters, Ensemble& iUnCorrected) const;
      void getDefaultParametersCore(Parameters& iParameters) const;
      void updateParametersCore(const std::vector<Ensemble>& iUnCorrected,
           const std::vector<Obs>& iObs,
           Parameters& iParameters) const;

      // Options
      int mNumWaves;
      int mNumPoints;
      bool mMultiplicative;
      std::string mVariable; // Which wind direction variable should be used?

      float computeCorrection(float iDirection, const Parameters& iParameters) const;
      //! Get wind direction valid at the same time as this ensemble
      float getDirection(const Ensemble& iEnsemble) const;
      int   getBinIndex(int iDirection) const;
};
#endif
