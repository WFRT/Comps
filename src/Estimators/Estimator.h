#ifndef ESTIMATOR_H
#define ESTIMATOR_H
#include "../Global.h"
#include "../Component.h"
class Obs;
class Probabilistic;
class Ensemble;
class Obs;


/** Must be insulated from other components. */
class Estimator : public Component {
   public:
      Estimator(const Options& iOptions, const Data& iData);
      std::string getParameters() const;
      //static Estimator* getScheme(const Options& iOptions, const Data& iData);
      //static Estimator* getScheme(const std::string& iTag, const Data& iData);
      //void update(const InstanceProbabilistic& iInstance, Parameters& iParameters) const;
      //! Don't use this function
      void getDefaultParameters(Parameters& iParameters);
      virtual void getDefaultParameters(const Parameters& iSchemeParameters, Parameters& iParameters) const = 0;
   private:
};

class EstimatorProbabilistic : public Estimator {
   public:
      EstimatorProbabilistic(const Options& iOptions, const Data& iData, const Probabilistic& iScheme);
      //! Must update their own parameters
      virtual void update(const Ensemble& iEnsemble, const std::vector<Obs>& iObs, Parameters& iParameters) const = 0;
      // This approach probably doesn't work, since we can't perturb iDist's parameters
      //virtual void update(const Distribution::ptr iDist, const std::vector<Obs>& iObs, Parameters& iParameters) const {};
      static EstimatorProbabilistic* getScheme(const Options& iOptions, const Data& iData, const Probabilistic& iScheme);
      void getCoefficients(const Parameters& iParameters, Parameters& iCoefficients) const;
   protected:
      //! How many coefficients are actually in iParameters? (i.e. how many are not estimator parameters)
      virtual int getNumCoefficients(const Parameters& iParameters) const = 0;
      const Probabilistic& mScheme;
};
#endif
