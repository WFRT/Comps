#include "Location.h"
#include "Inputs/Input.h"

Location::Location() : mId(0),
      mDataset(""),
      mLat(Global::MV),
      mLon(Global::MV),
      mElev(Global::MV),
      mLevel(0) {
}
Location::Location(const std::string& iDataset, int iId, 
      float iLat, float iLon, float iElev, std::string iName, int iLevel) :
   mId(iId), mDataset(iDataset),
   mLat(iLat), mLon(iLon),
   mElev(iElev), mName(iName), mLevel(iLevel) {
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

void Location::setValues(float iLat, float iLon, float iElev, int iLevel) {
   mLat = iLat;
   mLon = iLon;
   mElev = iElev;
   mLevel = iLevel;
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

void Location::setCode(std::string iCode) {
   mCode = iCode;
}

std::string Location::getCode() const {
   return mCode;
}
