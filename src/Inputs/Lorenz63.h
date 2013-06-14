#ifndef INPUT_LORENZ63_H
#define INPUT_LORENZ63_H
#include "Input.h"
#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>

class InputLorenz63 : public Input {
   public:
      InputLorenz63(const Options& rOptions, const Data& iData);
      ~InputLorenz63();
      float getValueCore(const Key::Input& iKey) const;
   private:
      void loadLocations() const;
      void loadMembers() const;
      std::string getFilename(int iDate) const;
      static void getDateTime(std::string iStamp, int& iDate, float& iTime);
      std::string mFilenamePrefix;
      std::string mFilenamePostfix;
      mutable Cache<int, std::vector<float> > mLocalCache; // time-step variables
      float mX0;
      float mY0;
      float mZ0;
      float mR;
      float mS;
      float mB;
      float mDt;
      int   mEnsSize;
      float mXVar;
      float mYVar;
      float mZVar;
      // Timestep to use for computing true value
      static const float mObsDt = 0.0001;
      mutable boost::variate_generator<boost::mt19937, boost::normal_distribution<> > mRand;
};
#endif
