#include "Sinusoidal.h"
#include "../Location.h"
#include "../Member.h"
#include "../Options.h"

std::map<int,float> InputSinusoidal::mDayPerturbation;

InputSinusoidal::InputSinusoidal(const Options& iOptions) : Input(iOptions),
      //mRand(boost::mt19937, boost::normal_distribution<>(0.0, 1.0)) {
      mRand(boost::mt19937(0), boost::normal_distribution<>()),
      mDayBiasStd(0),
      mDayBiasEfold(10),
      mDayCommonStd(0),
      mDayPeak(180),
      mHourPeak(12),
      mEnsStd(0) {

   //! Annual mean
   iOptions.getRequiredValue("mean", mMean);
   //! Difference between mean and peaks in the curve
   iOptions.getRequiredValue("yearAmplitude", mYearAmplitude);
   //! Distance between mean and peaks in the curve
   iOptions.getRequiredValue("dayAmplitude", mDayAmplitude);
   //! Number of ensemble members
   iOptions.getRequiredValue("members", mNumMembers);
   //! Standard deviation of day-to-day changes
   iOptions.getValue("dayCommonStd", mDayCommonStd);
   //! Standard deviation of bias of the day
   iOptions.getValue("dayBiasStd", mDayBiasStd);
   //! How many days should it take to make daily bias 1/e of its starting value
   iOptions.getValue("dayBiasEfold", mDayBiasEfold);
   //! Standard deviation of ensemble noise
   iOptions.getValue("ensStd", mEnsStd);
   //! What julian date is the peak?
   iOptions.getValue("dayPeak", mDayPeak);
   //! What offset is the peak?
   iOptions.getValue("hourPeak", mHourPeak);

   optimizeCacheOptions(); // Don't let user optimize cache

   if(getType() == typeObservation && mNumMembers > 1) {
      Global::logger->write("InputSinusoidal: Observation dataset cannot have 'members' > 1", Logger::error);
   }

   if(mDayBiasEfold <= 0) {
      std::stringstream ss;
      ss << "InputSinusoidal: DayBiasEfold myst be greater than 0";
      Global::logger->write(ss.str(), Logger::error);
   }
   iOptions.check();
   init();
}

float InputSinusoidal::getValueCore(const Key::Input& iKey) const {
   float returnValue = Global::MV;

   float dayPerturbation = 0;
   int yesterday = Global::getDate(iKey.date, 0, -24);
   std::map<int,float>::const_iterator it = mDayPerturbation.find(iKey.date);
   if(it != mDayPerturbation.end()) {
      dayPerturbation = it->second;
   }
   else {
      dayPerturbation = mRand();
      mDayPerturbation[iKey.date] = dayPerturbation;
   }

   float dayBias = 0;
   std::map<int,float>::const_iterator it2 = mDayBias.find(yesterday);
   if(it2 != mDayBias.end()) {
      dayBias = it2->second * (1 - 1.0/mDayBiasEfold)  + mRand() * mDayBiasStd;
   }
   else {
      dayBias = mRand() * mDayBiasStd;
   }
   mDayBias[iKey.date] = dayBias;

   int jd = Global::getJulianDay(iKey.date);
   float Tday  = mYearAmplitude * cos((jd - mDayPeak) / 365.0 * 2 * Global::pi);
   float Thour = mDayAmplitude  * cos((iKey.offset - mHourPeak) / 24 * 2 * Global::pi);

   Key::Input key = iKey;
   for(key.member = 0; key.member < mNumMembers; key.member++) {
      // Ensemble spread component
      float Tmember = mRand() * mEnsStd;
      float value   = mMean + Tday + Thour + Tmember + dayPerturbation * mDayCommonStd + dayBias;

      Input::addToCache(key, value);
      if(key == iKey)
         returnValue = value;
   }

   return returnValue;
}

void InputSinusoidal::getMembersCore(std::vector<Member>& iMembers) const {
   for(int i = 0; i < mNumMembers; i++) {
      Member member(getName(), i);
      iMembers.push_back(member);
   }
}

void InputSinusoidal::optimizeCacheOptions() {
   mCacheOtherMembers = true;
   mCacheOtherVariables = false;
   mCacheOtherOffsets = false;
   mCacheOtherLocations = false;
}
