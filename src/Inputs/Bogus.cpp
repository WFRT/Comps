#include "Bogus.h"
#include "../Location.h"
#include "../Member.h"
#include "../Options.h"

InputBogus::InputBogus(const Options& rOptions, const Data& iData) : Input(rOptions, iData),
      //mRand(boost::mt19937, boost::normal_distribution<>(0.0, 1.0)) {
      mRand(boost::mt19937(0), boost::normal_distribution<>()) {

   // Attributes
   rOptions.getRequiredValue("mean", mMean);
   rOptions.getRequiredValue("period", mPeriod);
   rOptions.getRequiredValue("amplitude", mAmplitude);
   rOptions.getRequiredValue("members", mNumMembers);
   if(!rOptions.getValue("speed", mSpeed)) {
      mSpeed = 0;
   }
   if(!rOptions.getValue("randVariance", mRandVariance)) {
      mRandVariance = 0;
   }
   if(mType == typeForecast) {
      rOptions.getRequiredValue("ensSpread", mEnsSpread);
   }
   else {
      mEnsSpread = Global::MV;
   }
   init();
}

InputBogus::~InputBogus() {
}

float InputBogus::getValueCore(const Key::Input& iKey) const {
   float pi = 3.14159265;
   // Spread component
   float sp;
   if(mType == typeForecast) {
      sp = ((float) iKey.member / mMembers.size() - 0.5) * mEnsSpread;
   }
   else 
      sp = 0;

   // Random component
   float e = 0;
   if(mRandVariance != 0) {
      e = mRand()*sqrt(mRandVariance);
   }
   if(mType == Input::typeObservation) {
      Input::addToCache(iKey, mMean);
      return mMean;
   }
   //return mMean + mAmplitude * sin(Global::getJulianDay(iKey.date) * mSpeed / 365 * 2* pi + iKey.offset / mPeriod * 2 * pi) + sp + e;
   float ensVariance = std::pow(mRand()*3,2);
   float a0 = 2.3;
   float a1 = 1.3;
   float error2 = a0 + a1*ensVariance;
   float error = std::pow((double) error2,(double) 0.5);
   float returnValue = Global::MV;
   for(int i = 0; i < mMembers.size(); i++) {
      Key::Input key = iKey;
      key.member = i;
      float value = mMean + std::pow((double) ensVariance,(double) 0.5)*mRand() + error;
      //float value = mMean + error;
      Input::addToCache(key, value);
      //std::cout << iKey.date << " " << iKey.offset << " " << key.member << " " << mType << value << std::endl;
      if(iKey.member == i) returnValue = value;
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

