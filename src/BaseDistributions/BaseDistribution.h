#ifndef BASE_DISTRIBUTION_H
#define BASE_DISTRIBUTION_H
#include "../Global.h"
#include "../Options.h"
#include "../Processor.h"

class BaseDistribution : public Processor {
   public:
      BaseDistribution(const Options& iOptions, const Data& iData);
      virtual float getCdf(float iX,   const std::vector<float>& iMoments) const = 0;
      virtual float getPdf(float iX,   const std::vector<float>& iMoments) const = 0;
      virtual float getInv(float iCdf, const std::vector<float>& iMoments) const = 0;
      virtual int   getNumMoments() const = 0;
      static        BaseDistribution* getScheme(const Options& iOptions, const Data& iData);
      static        BaseDistribution* getScheme(const std::string& iTag, const Data& iData);
      bool          isMandatory() const {return false;};
      virtual bool  needsTraining() const {return false;};
};
#endif

