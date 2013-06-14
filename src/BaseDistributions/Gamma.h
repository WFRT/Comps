#ifndef BASE_DISTRIBUTION_GAMMA_H
#define BASE_DISTRIBUTION_GAMMA_H
#include "../Global.h"
#include "../Options.h"
#include "BaseDistribution.h"

class BaseDistributionGamma : public BaseDistribution {
   public:
      BaseDistributionGamma(const Options& iOptions, const Data& iData);
      float getCdf(float iX, const std::vector<float>& iMoments) const;
      float getPdf(float iX, const std::vector<float>& iMoments) const;
      float getInv(float iCdf, const std::vector<float>& iMoments) const;
      int getNumMoments() const;
   private:
      static float getShape(float iMean, float iVariance);
      static float getScale(float iMean, float iVariance);
};
#endif

