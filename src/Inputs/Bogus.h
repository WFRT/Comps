#ifndef INPUT_BOGUS_H
#define INPUT_BOGUS_H
#include "Input.h"
#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>

//! Creates fake sinusoidal data of the form y(t) = m + sin(d*2*pi/365) + sp + e, where
//! m is the mean, d is the day of year, sp is noise due to ensemble variance, e is random noise
class InputBogus : public Input {
   public:
      InputBogus(const Options& iOptions, const Data& iData);
      void  getDates(std::vector<int>& rDates) const;
      void  loadMembers() const;
   private:
      float getValueCore(const Key::Input& iKey) const;
      float mMean;
      float mPeriod;
      float mAmplitude;
      float mSpeed;
      float mEnsVariance;
      float mRandVariance;
      int mNumMembers;
      mutable boost::variate_generator<boost::mt19937, boost::normal_distribution<> > mRand;
      void optimizeCacheOptions();
};
#endif
