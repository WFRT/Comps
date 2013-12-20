#ifndef INPUT_BOGUS_H
#define INPUT_BOGUS_H
#include "Input.h"
#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>

//! Creates fake sinusoidal data of the form y(t) = m + sin(d*2*pi/365) + sp + e, where
//! m is the mean, d is the day of year, sp is noise due to ensemble variance, e is random noise
class InputSinusoidal : public Input {
   public:
      InputSinusoidal(const Options& iOptions);
      bool  getDatesCore(std::vector<int>& iDates) const;
      void  getMembersCore(std::vector<Member>& iMembers) const;
   private:
      float getValueCore(const Key::Input& iKey) const;
      float mMean;
      float mPeriod;
      float mAmplitude;
      float mSpeed;
      float mEnsVariance;
      float mRandVariance;
      float mMin;
      float mMax;
      int mNumMembers;
      mutable boost::variate_generator<boost::mt19937, boost::normal_distribution<> > mRand;
      void optimizeCacheOptions();
};
#endif
