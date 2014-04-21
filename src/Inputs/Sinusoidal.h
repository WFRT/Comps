#ifndef INPUT_BOGUS_H
#define INPUT_BOGUS_H
#include "Input.h"
#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>

//! Creates fake sinusoidal data of the form:
//! y(t) = mean + annualAmplitude*cos(d*2*pi/365 - dayPeak) + hourlyAmplitude*sin(h*2*pi/24 - hourPeak) + noise
//! where 'd' is the day of year, 'h' is the hour of day (UTC), and 'noise' is made up of Gaussian
//! perturbations due to ensemble, day-to-day variability, and random noise.
//! e is random noise
class InputSinusoidal : public Input {
   public:
      InputSinusoidal(const Options& iOptions);
      void  getMembersCore(std::vector<Member>& iMembers) const;
   private:
      float getValueCore(const Key::Input& iKey) const;
      bool  hasDataFiles() const {return false;};
      float mMean;
      float mYearAmplitude;
      float mDayAmplitude;
      int   mDayPeak;
      float mHourPeak;
      float mEnsStd;
      float mDayBiasStd;
      float mDayCommonStd;
      float mDayBiasEfold;
      int   mNumMembers;
      mutable boost::variate_generator<boost::mt19937, boost::normal_distribution<> > mRand;
      void optimizeCacheOptions();
      static std::map<int,float>  mDayPerturbation; // date, perturbation
      mutable std::map<int,float> mDayBias;         // date, perturbation
};
#endif
