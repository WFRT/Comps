#ifndef CONTINUOUS_BMA_H
#define CONTINUOUS_BMA_H
#include "../Global.h"
#include "../Options.h"
#include "../BaseDistributions/BaseDistribution.h"
#include "../Scheme.h"

//! Date: 20130305
//! Author: Dominique Bourdin
//! Bayesian Model Averaging
//! Adaptive parameter estimation based on
//! http://www.dtcenter.org/events/workshops12/nuopc_2012/Presentations/Veenhuis%20-%20Ensemble%20Workshop.pdf
//! NOTE: Assuming individual first moments, and pooled second moments
class ContinuousBma : public Continuous {
   public:
      ContinuousBma(const Options& rOptions, const Data& iData);
      ~ContinuousBma();
      //! The ensemble size must be constant, because there is one weight parameter per member
      bool needsConstantEnsembleSize() const {return true;};
   protected:
      float getCdfCore(float iX, const Ensemble& iEnsemble, const Parameters& iParameters) const;
      float getPdfCore(float iX, const Ensemble& iEnsemble, const Parameters& iParameters) const;
      void getMoments(const Ensemble& iEnsemble, const Parameters& iParameters, std::vector<float>& iMoments) const;
      void getDefaultParametersCore(Parameters& iParameters) const;
      void updateParametersCore(const Ensemble& iEnsemble, const Obs& iObs, Parameters& iParameters) const;

      BaseDistribution* mBaseDistribution;
};
#endif
