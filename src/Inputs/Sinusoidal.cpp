#include "Sinusoidal.h"
#include "../Location.h"
#include "../Member.h"
#include "../Options.h"

InputSinusoidal::InputSinusoidal(const Options& iOptions, const Data& iData) : Input(iOptions, iData),
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

   if(getType() == typeObservation && mNumMembers > 1) {
      Global::logger->write("InputSinusoidal: Observation dataset cannot have 'members' > 1", Logger::error);
   }
   init();
}

float InputSinusoidal::getValueCore(const Key::Input& iKey) const {
   float pi = 3.14159265;
   float returnValue = Global::MV;

   // Random component
   float e = mRand()*sqrt(mRandVariance);

   Key::Input key = iKey;
   for(key.member = 0; key.member < mNumMembers; key.member++) {
      // Ensemble spread component
      float sp = 0;
      if(getType() == typeForecast) {
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

bool  InputSinusoidal::getDatesCore(std::vector<int>& iDates) const {
   int date = 20120101;
   while(date < 20130101) {
      iDates.push_back(date);
      date = Global::getDate(date, 0, 24);
   }
   return true;
}

void InputSinusoidal::getMembersCore(std::vector<Member>& iMembers) const {
   for(int i = 0; i < mNumMembers; i++) {
      Member member(getName(), 0, "", i);
      iMembers.push_back(member);
   }
}

void InputSinusoidal::optimizeCacheOptions() {
   mCacheOtherMembers = true;
   mCacheOtherVariables = false;
   mCacheOtherOffsets = false;
   mCacheOtherLocations = false;
}
