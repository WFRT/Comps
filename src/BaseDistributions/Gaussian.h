#ifndef BASE_DISTRIBUTION_GAUSSIAN_H
#define BASE_DISTRIBUTION_GAUSSIAN_H
#include "../Global.h"
#include "../Options.h"
#include "BaseDistribution.h"

class BaseDistributionGaussian : public BaseDistribution {
   public:
      BaseDistributionGaussian(const Options& iOptions, const Data& iData);
      float getCdf(float iX,   const std::vector<float>& iMoments) const;
      float getPdf(float iX,   const std::vector<float>& iMoments) const;
      float getInv(float iCdf, const std::vector<float>& iMoments) const;
      int getNumMoments() const;
};
#endif

