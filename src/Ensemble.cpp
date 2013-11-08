#include "Ensemble.h"

const int Ensemble::mNumCachedMoments = 2;

Ensemble::Ensemble() : mVariable(""),
      mDate(0),
      mInit(0),
      mOffset(0) {
   resetCachedMoments();
}

Ensemble::Ensemble(std::vector<float> iValues, std::string iVariable) :
      mValues(iValues), mVariable(iVariable) {
}

Ensemble::Ensemble(std::vector<float> iValues, std::vector<float> iSkills, std::string iVariable) :
      mValues(iValues), mSkills(iSkills), mVariable(iVariable) {
   assert(iValues.size() == mSkills.size());
}

float& Ensemble::operator[](unsigned int i) {
   resetCachedMoments();
   if(i >= Ensemble::maxSize) {
      std::stringstream ss;
      ss << "Ensemble: Maximum ensemble size reached: " << Ensemble::maxSize << std::endl;
      Global::logger->write(ss.str(), Logger::error);
   }
   // Resize
   if(i >= mValues.size()) {
      mValues.resize(i+1, Global::MV);
   }
   return mValues[i];
}

std::vector<float> Ensemble::getValues() const {
   return mValues;
}

std::vector<float> Ensemble::getSkills() const {
   if(mValues.size() != mSkills.size())
   for(int i = 0; i < (int) mValues.size(); i++) {
      mSkills.push_back(Global::MV);
   }
   return mSkills;
}

int Ensemble::size() const {
   return mValues.size();
}

float Ensemble::getMin() const {
   float min = Global::INF;
   bool found = false;
   for(int i = 0; i < mValues.size(); i++) {
      float value = mValues[i];
      if(Global::isValid(value) && value < min) {
         min = mValues[i];
         found = true;
      }
   }
   if(!found)
      min = Global::MV;
   return min;
}
float Ensemble::getMax() const {
   float max = -Global::INF;
   bool found = false;
   for(int i = 0; i < mValues.size(); i++) {
      float value = mValues[i];
      if(Global::isValid(value) && value > max) {
         max = mValues[i];
         found = true;
      }
   }
   if(!found)
      max = Global::MV;
   return max;
}

float const& Ensemble::operator[](unsigned int i) const {
   if(i >= mValues.size()) {
      std::stringstream ss;
      ss << "Ensemble: Index " << i << " out of bounds (" << mValues.size() << ")" << std::endl;
      Global::logger->write(ss.str(), Logger::error);
   }
   return mValues[i];
}
void Ensemble::setSkills(std::vector<float> iSkills) {
   mSkills = iSkills;
}
void Ensemble::setValues(std::vector<float> iValues) {
   resetCachedMoments();
   mValues = iValues;
}
void Ensemble::setVariable(std::string iVariable) {
   mVariable = iVariable;
}
void Ensemble::setSize(int iSize) {
   mValues.resize(iSize, Global::MV);
}

void Ensemble::setInfo(int iDate, int iInit, float iOffset, Location iLocation, std::string iVariable) {
   mDate = iDate;
   mInit = iInit;
   mOffset = iOffset;
   mLocation = iLocation;
   mVariable = iVariable;
}

int Ensemble::getDate() const {
   return mDate;
}
int Ensemble::getInit() const {
   return mInit;
}
float Ensemble::getOffset() const {
   return mOffset;
}
Location Ensemble::getLocation() const {
   return mLocation;
}
std::string Ensemble::getVariable() const {
   return mVariable;
}

float Ensemble::getMoment(int iMoment) const {
  
   if(iMoment == 0)
      return Global::MV;
   if(iMoment <= mNumCachedMoments) {
      int index = iMoment-1;
      if(Global::isValid(mMoments[index])) {
         // Already cached
         //std::cout << "CACHE HIT " << iMoment << std::endl;
         return mMoments[index];
      }
      else {
         float moment = Global::getMoment(mValues, iMoment);
         //std::cout << "Caching moment " << iMoment << std::endl;
         // Cache the value
         mMoments[index] = moment;
         return moment;
      }
   }
   else {
      // Don't cache the moment
         //std::cout << "Not cached moment " << iMoment << std::endl;
      return Global::getMoment(mValues, iMoment);
   }
}

void Ensemble::resetCachedMoments() const {
   mMoments.clear();
   mMoments.resize(mNumCachedMoments, Global::MV);
}
