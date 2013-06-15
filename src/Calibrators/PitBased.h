#ifndef CALIBRATOR_PITBASED_H
#define CALIBRATOR_PITBASED_H
#include "Calibrator.h"
class Interpolator;

class CalibratorPitBased : public Calibrator {
   public:
      CalibratorPitBased(const Options& iOptions, const Data& iData);
      ~CalibratorPitBased();
      float calibrate(float iCdf, const Distribution::ptr iDist, const Parameters& iParameters) const;
      float unCalibrate(float iCdf, const Distribution::ptr iDist, const Parameters& iParameters) const;
      float amplify(float iCdf, const Distribution::ptr iDist, const Parameters& iParameters) const;
      void  getDefaultParameters(Parameters& iParameters) const;
      void  updateParameters(const std::vector<Distribution::ptr> iDist, const std::vector<Obs>& iObs, Parameters& iParameters) const;
   private:
      std::vector<float> mPit;
      std::vector<float> mPit0;
      std::vector<float> mPit1;
      int mNumSmooth;
      int mNumSmooth0;
      int mNumSmooth1;
      bool mDoDiscrete;
      bool mUseSpline;
      enum Type {typeContinuous = 0, typeLower = 10, typeUpper = 20};
      void subsetParameters(const Parameters& iAllParameters, std::map<Type, Parameters>& iParameterMap) const;
      void packageParameters(const std::map<Type, Parameters>& iParameterMap, Parameters& iAllParameters) const;
      bool isFlatEnough(const Parameters& iCurve) const;
      float mInteliCalFactor;
      bool mInteliCal;
      Interpolator* mInterpolator;
};
#endif

