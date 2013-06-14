#include "Gaussian.h"
#include <boost/math/distributions/normal.hpp>

BaseDistributionGaussian::BaseDistributionGaussian(const Options& iOptions, const Data& iData) : BaseDistribution(iOptions, iData) {

}
float BaseDistributionGaussian::getCdf(float iX, const std::vector<float>& iMoments) const {
   assert(iMoments.size() == 2);
   float mean     = iMoments[0];
   float variance = iMoments[1];
   if(variance == 0) {
      return Global::MV;
   }
   boost::math::normal dist(mean, sqrt(variance));
   return boost::math::cdf(dist, iX);
}
float BaseDistributionGaussian::getPdf(float iX, const std::vector<float>& iMoments) const {
   assert(iMoments.size() == 2);
   float mean     = iMoments[0];
   float variance = iMoments[1];
   if(variance == 0) {
      return Global::MV;
   }
   boost::math::normal dist(mean, sqrt(variance));
   return boost::math::pdf(dist, iX);
}
float BaseDistributionGaussian::getInv(float iCdf, const std::vector<float>& iMoments) const {
   assert(iMoments.size() == 2);
   float mean     = iMoments[0];
   float variance = iMoments[1];
   if(variance == 0) {
      return Global::MV;
   }
   boost::math::normal dist(mean, sqrt(variance));
   return boost::math::quantile(dist, iCdf);

}
int BaseDistributionGaussian::getNumMoments() const {
   return 2;
}
