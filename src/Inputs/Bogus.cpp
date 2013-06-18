#include "Bogus.h"
#include "../Location.h"
#include "../Member.h"
#include "../Options.h"

InputBogus::InputBogus(const Options& iOptions, const Data& iData) : Input(iOptions, iData),
      //mRand(boost::mt19937, boost::normal_distribution<>(0.0, 1.0)) {
      mRand(boost::mt19937(0), boost::normal_distribution<>()),
      mSpeed(0),
      mRandVariance(0),
      mEnsVariance(0),
      mMin(Global::MV),
      mMax(Global::MV) {

   //! Mean of the dataset
   iOptions.getRequiredValue("mean", mMean);
   //! Time between successive peaks in the cuve (in hours)
   iOptions.getRequiredValue("period", mPeriod);
   //! Distance between mean and peaks in the curve
   iOptions.getRequiredValue("amplitude", mAmplitude);
   //! Number of ensemble members
   iOptions.getRequiredValue("members", mNumMembers);
   //! How fast does the wave travel forward in time (in days)
   iOptions.getValue("speed", mSpeed);
   //! Variance of random noise
   iOptions.getValue("randVariance", mRandVariance);
   //! Variance of ensemble
   iOptions.getValue("ensVariance", mEnsVariance);
   //! Minimum value
   iOptions.getValue("min", mMin);
   //! Maximum value
   iOptions.getValue("max", mMax);


   optimizeCacheOptions(); // Don't let user optimize cache

   if(mType == typeObservation && mNumMembers > 1) {
      Global::logger->write("InputBogus: Observation dataset cannot have 'members' > 1", Logger::error);
   }
   init();
}

float InputBogus::getValueCore(const Key::Input& iKey) const {
   float pi = 3.14159265;
   float returnValue = Global::MV;

   // Random component
   float e = mRand()*sqrt(mRandVariance);

   Key::Input key = iKey;
   for(key.member = 0; key.member < mNumMembers; key.member++) {
      // Ensemble spread component
      float sp = 0;
      if(mType == typeForecast) {
         sp = mRand() * sqrt(mEnsVariance);
         //sp = ((float) iKey.member / mMembers.size() - 0.5) * mEnsVariance;
      }

      float amplitude = mAmplitude * sin(Global::getJulianDay(iKey.date) * mSpeed / 365 * 2* pi + iKey.offset / mPeriod * 2 * pi);
      float value = mMean + amplitude + sp + e;
      if(Global::isValid(mMin) && value < mMin)
         value = mMin;
      if(Global::isValid(mMax) && value > mMax)
         value = mMax;

      assert(Global::isValid(value));

      Input::addToCache(key, value);
      if(key == iKey)
         returnValue = value;
   }

   return returnValue;
}

void  InputBogus::getDates(std::vector<int>& rDates) const {
   for(int i = 20120101; i < 20120131; i++) {
      rDates.push_back(i);
   }
}

void InputBogus::loadMembers() const {
   for(int i = 0; i < mNumMembers; i++) {
      Member member(mName, 0, "", i);
      mMembers.push_back(member);
   }
}

void InputBogus::optimizeCacheOptions() {
   mCacheOtherMembers = true;
   mCacheOtherVariables = false;
   mCacheOtherOffsets = false;
   mCacheOtherLocations = false;
}
