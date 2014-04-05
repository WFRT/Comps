#ifndef DISCRETE_H
#define DISCRETE_H
#include "../Probabilistic.h"
#include "../Options.h"
#include "../Data.h"
#include "../Ensemble.h"
#include "../Estimators/Estimator.h"

//! Schemes that represent the probability mass at a discrete point in the variable's domain.
class Discrete : public Probabilistic {
   public:
      Discrete(const Options& iOptions, const Data& iData);
      float  getP(const Ensemble& iEnsemble, const Parameters& iParameters) const;
      void   updateParameters(const std::vector<Ensemble>& iEnsemble, const std::vector<Obs>& iObs, Parameters& iParameters) const;
      static Discrete* getScheme(const Options& iOptions, const Data& iData);
      static Discrete* getScheme(const std::string& iTag, const Data& iData);
      bool   isMemberDependent() const {return false;};
      enum   BoundaryType {TypeLower, TypeUpper};
      //! Combines the parameters from the scheme and its estimator (if applicable)
      void   getDefaultParameters(Parameters& iParameters) const;
      int    numParameters() const;

      float  getLikelihood(float iObs, const Ensemble& iEnsemble, const Parameters& iParameters) const;
      virtual bool needsConstantEnsembleSize() const {return false;};
      float getX() const;
   protected:
      virtual float getPCore(const Ensemble& iEnsemble, const Parameters& iParameters) const = 0;
      virtual void updateParametersCore(
            const std::vector<Ensemble>& iEnsemble,
            const std::vector<Obs>& iObs,
            Parameters& iParameters) const;
      virtual void  getDefaultParametersCore(Parameters& iParameters) const {return;}
      float mX; // Location of discrete mass
      static float mMinP;
};
#endif

