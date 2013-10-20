#ifndef INPUT_LORENZ63_H
#define INPUT_LORENZ63_H
#include "Input.h"
#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>

//! Simulates the Lorenz 1963 system of ordinary differential equations
class InputLorenz63 : public Input {
   public:
      InputLorenz63(const Options& iOptions, const Data& iData);
      ~InputLorenz63();
      float getValueCore(const Key::Input& iKey) const;
   private:
      void        getMembersCore(std::vector<Member>& iMembers) const;
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
      mutable boost::variate_generator<boost::mt19937, boost::normal_distribution<> > mRand;
};
#endif
