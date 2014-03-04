#ifndef CONTINUOUS_MOMENTS_H
#define CONTINUOUS_MOMENTS_H
#include "Continuous.h"

class BaseDistribution;
class Measure;
class Transform;

class ContinuousMoments : public Continuous {
   public:
      ContinuousMoments(const Options& iOptions, const Data& iData);
      ~ContinuousMoments();
      bool needsTraining() const;
   protected:
      float getCdfCore(float iX, const Ensemble& iEnsemble, const Parameters& iParameters) const;
      float getPdfCore(float iX, const Ensemble& iEnsemble, const Parameters& iParameters) const;
      float getMomentCore(int iMoment, const Ensemble& iEnsemble, const Parameters& iParameters) const;
      std::string mType;
      BaseDistribution* mBaseDistribution;
      void getDefaultParametersCore(Parameters& iParameters) const;
      int numParametersCore() const {return 4;};
      bool getMoments(const Ensemble& iEnsemble,
            const Parameters& iParameters,
            std::vector<float>& iMoments) const;
      void updateParametersCore(const std::vector<Ensemble>& iEnsemble, const std::vector<Obs>& iObs, Parameters& iParameters) const;
      Measure* mMeasure;
      Transform* mTransform;
};
#endif
