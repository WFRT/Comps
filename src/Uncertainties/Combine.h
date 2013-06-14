#ifndef UNCERTAINTY_COMBINE_H
#define UNCERTAINTY_COMBINE_H
#include "Uncertainty.h"

class UncertaintyCombine : public Uncertainty {
   public:
      UncertaintyCombine(const Options& iOptions, const Data& iData);
      ~UncertaintyCombine();
      void getDistribution(const Ensemble& iEnsemble, const Parameters& iParameters, Distribution& iDistribution);
      void updateParameters(const Ensemble& iEnsemble, const Obs& iObs, Parameters& iParameters) const;
      void getDefaultParameters(Parameters& iParameters) const;
      bool needsConstantEnsembleSize() const;
      bool needsTraining() const;
   protected:
      float getCdf(float iX, const Ensemble& iEnsemble, const Parameters& iParameters) const;
      float getPdf(float iX, const Ensemble& iEnsemble, const Parameters& iParameters) const;
      float getInv(float iCdf, const Ensemble& iEnsemble, const Parameters& iParameters) const;
      float getMoment(int iMoment, const Ensemble& iEnsemble, const Parameters& iParameters) const;
      Continuous* mContinuous;
      Discrete*   mDiscreteLower;
      Discrete*   mDiscreteUpper;
      bool mDoContinuous;
      bool mDoLower;
      bool mDoUpper;
      bool mDoOnlyDiscrete;
      enum Type {typeContinuous = 0, typeLower = 10, typeUpper = 20};
      void subsetParameters(const Parameters& iAllParameters, std::map<UncertaintyCombine::Type, Parameters>& iParameterMap) const;
      void packageParameters(const std::map<UncertaintyCombine::Type, Parameters>& iParameterMap, Parameters& iAllParameters) const;
};
#endif

