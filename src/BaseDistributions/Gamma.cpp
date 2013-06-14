#include "Gamma.h"
#include <boost/math/distributions/gamma.hpp>

BaseDistributionGamma::BaseDistributionGamma(const Options& iOptions, const Data& iData) : BaseDistribution(iOptions, iData) {

}
float BaseDistributionGamma::getCdf(float iX, const std::vector<float>& iMoments) const {
   assert(iMoments.size() == 2);
   float mean     = iMoments[0];
   float variance = iMoments[1];
   if(variance == 0) {
      return Global::MV;
   }
   float scale    = getScale(mean, variance);
   float shape    = getShape(mean, variance);
   boost::math::gamma_distribution<> dist(shape, scale);
   return boost::math::cdf(dist, iX);
}
float BaseDistributionGamma::getPdf(float iX, const std::vector<float>& iMoments) const {
   assert(iMoments.size() == 2);
   float mean     = iMoments[0];
   float variance = iMoments[1];
   if(variance == 0) {
      return Global::MV;
   }
   float scale    = getScale(mean, variance);
   float shape    = getShape(mean, variance);
   //std::cout << "Scale, shape = " << scale << " " << shape << std::endl;
   boost::math::gamma_distribution<> dist(shape, scale);
   //std::cout << scale << " " << shape << std::endl;
   return boost::math::pdf(dist, iX);
}
float BaseDistributionGamma::getInv(float iCdf, const std::vector<float>& iMoments) const {
   assert(iMoments.size() == 2);
   float mean     = iMoments[0];
   float variance = iMoments[1];
   if(variance == 0) {
      return Global::MV;
   }
   float scale    = getScale(mean, variance);
   float shape    = getShape(mean, variance);
   boost::math::gamma_distribution<> dist(shape, scale);
   return boost::math::quantile(dist, iCdf);
}
int BaseDistributionGamma::getNumMoments() const {
   return 2;
}
float BaseDistributionGamma::getShape(float iMean, float iVariance) {
   // alpha (wiki: k)
   return iMean*iMean / iVariance;
}
float BaseDistributionGamma::getScale(float iMean, float iVariance) {
   // beta (wiki: theta)
   return iVariance / iMean;
}
