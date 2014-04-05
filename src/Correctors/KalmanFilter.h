#ifndef CORRECTOR_KALMAN_FILTER_H
#define CORRECTOR_KALMAN_FILTER_H
#include "Corrector.h"

class CorrectorKalmanFilter : public Corrector {
   public:
      CorrectorKalmanFilter(const Options& iOptions, const Data& iData);
   private:
      void correctCore(const Parameters& iParameters, Ensemble& iUnCorrected) const;
      void getDefaultParametersCore(Parameters& iParameters) const;
      int numParametersCore() const {return 8;};
      void updateParametersCore(const std::vector<Ensemble>& iUnCorrected,
           const std::vector<Obs>& iObs,
           Parameters& iParameters) const;
      float mRatio;
      static float mVarVarV;
      static float mVarVarW;
      static float mMaxP;
};
#endif
