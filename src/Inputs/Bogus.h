#ifndef INPUT_BOGUS_H
#define INPUT_BOGUS_H
#include "Input.h"
#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>

class InputBogus : public Input {
   public:
      InputBogus(const Options& rOptions, const Data& iData);
      ~InputBogus();
      void  getDates(std::vector<int>& rDates) const;
      void  loadMembers() const;
   private:
      float getValueCore(const Key::Input& iKey) const;
      float mMean;
      float mPeriod; // In number of offsets
      float mAmplitude;
      float mSpeed; // In number of days
      float mEnsSpread;
      float mRandVariance;
      int mNumMembers;
      mutable boost::variate_generator<boost::mt19937, boost::normal_distribution<> > mRand;
};
#endif
