#include "Location.h"
#include "Inputs/Input.h"

Location::Location() : mId(0),
      mDataset(""),
      mLat(Global::MV),
      mLon(Global::MV),
      mElev(Global::MV),
      mLandUse(Global::MV),
      mLandFraction(Global::MV),
      mCode(""),
      mLevel(0) {
}
Location::Location(const std::string& iDataset, int iId, float iLat, float iLon) :
      mId(iId), mDataset(iDataset),
      mLat(iLat), mLon(iLon),
      mElev(Global::MV),
      mLandUse(Global::MV),
      mLandFraction(Global::MV),
      mCode(""),
      mLevel(0) {
}
int Location::getId() const {
   return mId;
}
float Location::getLat() const {
   return mLat;
}
float Location::getLon() const {
   return mLon;
}
float Location::getElev() const {
   return mElev;
}
float Location::getLandFraction() const {
   return mLandFraction;
}
int Location::getLandUse() const {
   // TODO
   return 1;
}
std::string Location::getName() const {
   return mName;
}
int Location::getLevel() const {
   return mLevel;
}

std::string Location::getDataset() const {
   return mDataset;
}

float Location::getDistance(const Location& iLocation) const {
   return getDistance(iLocation.getLat(), iLocation.getLon(), getLat(), getLon());
}
float Location::getDistance(float lat1, float lon1, float lat2, float lon2) {
   if(Global::isMissing(lat1) || Global::isMissing(lat2) ||
      Global::isMissing(lon1) || Global::isMissing(lon2)) {
      return Global::MV;
   }
   assert(Global::isValid(lat1) && Global::isValid(lat2) && Global::isValid(lon1) && Global::isValid(lon2));
   assert(fabs(lat1) <= 90 && fabs(lat2) <= 90 && fabs(lon1) <= 360 && fabs(lon2) <= 360);

   //std::cout << "Distance: [" << lat1 << ", " << lon1 << "] to [" << lat2 << ", " << lon2 << "] = ";
   if(lat1 == lat2 && lon1 == lon2)
      return 0;
   double lat1r = deg2rad(lat1);
   double lat2r = deg2rad(lat2);
   double lon1r = deg2rad(lon1);
   double lon2r = deg2rad(lon2);
   double angle = cos(lat1r)*cos(lon1r)*cos(lat2r)*cos(lon2r) + cos(lat1r) * sin(lon1r) *cos(lat2r)*sin(lon2r) + sin(lat1r)*sin(lat2r);
   if(angle < -1 || angle > 1) {
      std::stringstream ss;
      ss  << "Location.cpp: Distance: " << angle << " [" << lat1 << ", " << lon1 << "] to [" << lat2 << ", " << lon2 << "]" << std::endl;
      Global::logger->write(ss.str(), Logger::error);

   }
   double dist = acos(angle)*mRadiusEarth;
   //std::cout << dist << "m" << std::endl;
   return (float) dist;
}
float Location::deg2rad(float deg) {
   return (deg * Global::pi / 180);
}
float Location::rad2deg(float rad) {
   return (rad * 180 / Global::pi);
}

bool Location::operator<(const Location &right) const {
   if(mLat == right.mLat) {
      if(mLon == right.mLon) {
         return mElev < right.mElev;
      }
      else {
         return mLon < right.mLon;
      }
   }
   else {
      return mLat < right.mLat;
   }
}
int Location::size() const {
   return sizeof(int) + mDataset.size() * sizeof(char);
}

std::string Location::getCode() const {
   return mCode;
}

void Location::setId(float iId) {mId = iId;};
void Location::setDataset(std::string iDataset) {mDataset = iDataset;};
void Location::setLat(float iLat) {mLat = iLat;};
void Location::setLon(float iLon) {mLon = iLon;};
void Location::setElev(float iElev) {mElev = iElev;};
void Location::setLevel(float iLevel) {mLevel = iLevel;};
void Location::setLandUse(int iLandUse) {mLandUse = iLandUse;};
void Location::setLandFraction(float iLandFraction) {mLandFraction = iLandFraction;};
void Location::setName(std::string iName) {mName = iName;};
void Location::setCode(std::string iCode) {mCode = iCode;};
