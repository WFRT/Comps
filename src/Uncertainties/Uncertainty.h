#ifndef UNCERTAINTY_H
#define UNCERTAINTY_H
#include "../Processor.h"
#include "../Distribution.h"
#include <memory>
class Options;
class Data;
class Parameters;
class Ensemble;
class Continuous;
class Discrete;
class Obs;

class Uncertainty : public Processor {
   public:
      Uncertainty(const Options& iOptions, const Data& iData);
      Distribution::ptr getDistribution(Ensemble iEnsemble, const Averager& iAverager, Parameters iParameters) const;
      static Uncertainty* getScheme(const Options& iOptions, const Data& iData);
      static Uncertainty* getScheme(const std::string& iTag, const Data& iData);
      virtual float getCdf(float iX, const Ensemble& iEnsemble, const Parameters& iParameters) const = 0;
      virtual float getPdf(float iX, const Ensemble& iEnsemble, const Parameters& iParameters) const = 0;
      virtual float getInv(float iCdf, const Ensemble& iEnsemble, const Parameters& iParameters) const = 0;
      virtual float getMoment(int iMoment, const Ensemble& iEnsemble, const Parameters& iParameters) const = 0;
      bool  isMemberDependent() const {return false;};
      virtual void updateParameters(const std::vector<Ensemble>& iEnsemble, const std::vector<Obs>& iObs, Parameters& iParameters) const = 0;
      virtual void getDefaultParameters(Parameters& iParameters) const = 0;
      virtual bool needsConstantEnsembleSize() const = 0;
   protected:
};
#endif

