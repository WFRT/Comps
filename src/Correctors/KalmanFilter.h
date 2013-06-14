#ifndef CORRECTOR_KALMAN_FILTER_H
#define CORRECTOR_KALMAN_FILTER_H
#include "Corrector.h"

class CorrectorKalmanFilter : public Corrector {
   public:
      CorrectorKalmanFilter(const Options& iOptions, const Data& iData);
   protected:
      void correctCore(const Parameters& iParameters, Ensemble& iUnCorrected) const;
      void getDefaultParametersCore(Parameters& iParameters) const;
      void updateParametersCore(const std::vector<Ensemble>& iUnCorrected,
           const std::vector<Obs>& iObs,
           Parameters& iParameters) const;
   private:
      float mRatio;
      const static float mVarVarV = 1;
      const static float mVarVarW = 0.0005;
      const static float mMaxP    = 10000;
};
#endif
