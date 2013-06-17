#include "SchemesHeader.inc"
#include "Input.h"
#include "../Variables/Variable.h"
#include "../Options.h"
#include "../Location.h"
#include "../Member.h"
#include "../Loggers/Logger.h"
#include "../DataKey.h"
#include "../DataKey.h"
#include "../Obs.h"
#include "../Ensemble.h"

/*********
 * Input *
 ********/
Input::Input(const Options& iOptions, const Data& iData) : Component(iOptions, iData), 
      mCacheOtherOffsets(false),
      mCacheOtherLocations(false),
      mCacheOtherMembers(false), 
      mCacheOtherVariables(false), 
      mOptimizeCache(false),
      mAllowTimeInterpolation(false),
      mHasWarnedCacheMiss(false),
      mFileExtension(""),
      mDoQc(true),
      mHasInit(false),
      mFilenameDateStartIndex(0),
      mMaxCacheSize(Global::MV) {
   // Process options
   iOptions.getRequiredValue("tag", mName);
   //! Name of folder that data resides in ./input/
   iOptions.getRequiredValue("folder", mFolder);
   iOptions.getValue("fileExtension", mFileExtension);
   //! Should this dataset allow values to be interpolated for times between its offsets?
   iOptions.getValue("allowTimeInterpolation", mAllowTimeInterpolation);
   //! When a value is retrived, should all other variables at the same location/member/offset be
   //! cached?
   iOptions.getValue("cacheOtherVariables", mCacheOtherVariables);
   iOptions.getValue("cacheOtherLocations", mCacheOtherLocations);
   iOptions.getValue("cacheOtherMembers", mCacheOtherMembers);
   iOptions.getValue("cacheOtherOffsets", mCacheOtherOffsets);
   //! Should the dataset figure out how to optimize the cache options itself?
   if(iOptions.getValue("optimize", mOptimizeCache)) {
      optimizeCacheOptions();
   }

   bool skipQc;
   //! Should quality control of values in the dataset be skipped?
   if(iOptions.getValue("skipQc", skipQc)) {
      mDoQc = !skipQc;
   }

   // Type
   std::string type;
   //! Dataset type. One of 'forecast' or 'observation'
   iOptions.getRequiredValue("type", type);
   if(type == "observation")
      mType = typeObservation;
   else if(type == "forecast")
      mType = typeForecast;
   else
      Global::logger->write("Unrecognized input type", Logger::error);
   // Directories
   std::stringstream ss;
   ss << "input/" << mFolder << "/";
   mDirectory  = ss.str();
   //! Location of data files
   if(!iOptions.getValue("dataDir", mDataDirectory)) {
      ss << "data/";
      mDataDirectory  = ss.str();
   }
   mIsDatesCached = false;
   iOptions.getValues("locations", mAllowLocations);

   std::stringstream ss0;
   ss0 << mName << ":Loc";
   mCacheSurroundingLocations.setName(ss0.str());
   mCacheNearestLocation.setName(ss0.str());

   mCache.setName(mName);

}
void Input::init() {
   assert(!mHasInit);
   // Offsets
   loadOffsets();
   makeOffsetMap();

   // Variables
   loadVariables();

   // Members
   loadMembers();
   // Check that obs dataset only has one member
   if(mType == Input::typeObservation) {
      if(mMembers.size() != 1) {
         std::stringstream ss;
         ss << "Input " << mName << " has " << mMembers.size() << " members. Must be 1";
         Global::logger->write(ss.str(), Logger::error);
      }
   }

   // Locations
   loadLocations();
   if(mAllowLocations.size() > 0) {
      std::vector<float>::iterator it;
      for(int j = (int) mLocations.size() - 1; j >= 0; j--) {
         bool found = false;
         for(int i = 0; i < (int) mAllowLocations.size(); i++) {
            if(mAllowLocations[i] == mLocations[j].getId()) {
               found = true;
            }
         }
         if(!found) {
            mLocations.erase(mLocations.begin() + j);
         }
      }
   }
   makeLocationMap();

   if(mOptimizeCache) {
      optimizeCacheOptions();
   }


   mHasInit = true;
}
std::string Input::getInputDirectory() {
   return "./input/";
}

Input::~Input() {
}
#include "Schemes.inc"

void Input::getAvailableVariables(std::vector<std::string>& rVariables) const {
   std::map<std::string,std::string>::iterator it;
   for(it = mVariableRename.begin(); it != mVariableRename.end(); it++) {
      rVariables.push_back(it->first);
   }
}
std::string Input::getConfigFilename(std::string type) const {
   std::stringstream ss(std::stringstream::out);
   ss << mDirectory << type << "." << mFileExtension;
   //ss << "options/" << type << ".nl";
   return ss.str();
}
std::string Input::getLocalVariableName(std::string rVariable) const {
   return mVariableRename[rVariable];
}
int Input::getNumMembers() const {
   return (int) mMembers.size();
}
int Input::getNumOffsets() const {
   return (int) mOffsets.size();
}
bool Input::hasVariable(std::string iVariableName) const {
   std::map<std::string, int>::const_iterator it = mVariableMap.find(iVariableName);
   return it != mVariableMap.end();
}

bool Input::hasOffset(float iOffset) const {
   std::map<float, int>::const_iterator it = mOffsetMap.find(iOffset);
   return it != mOffsetMap.end();
}

float Input::getValue(int rDate, int rInit, float iOffset, int iLocationNum, int rMemberId, std::string rVariable) const {
   // Check that inputs make sense
   int locationId = mLocationMap[iLocationNum];

   // Since obs from the same valid times (but different dates/offsets), we can try to find
   // another offset from a different day if the desired offset doesn't exist
   if(mType == Input::typeObservation && !Global::isValid(getOffsetIndex(iOffset))) {
      for(int i = 0; i < mOffsets.size(); i++) {
         if(abs(mOffsets[i] - iOffset) % 24 == 0) {
            rDate = Global::getDate(rDate, rInit, iOffset - mOffsets[i]);
            iOffset =  mOffsets[i];
         }
      }
   }
   /*
   if(mType == Input::typeObservation && iOffset >= 24) {
      rDate = Global::getDate(rDate, rInit, iOffset);
      iOffset =  fmod(iOffset, 24);
   }
   else if(mType == Input::typeObservation && iOffset < 0) {
      rDate = Global::getDate(rDate, rInit, iOffset);
      iOffset = Global::getOffset(rDate, iOffset);
   }
  */

   float value = Global::MV;
   int variableId = mVariable2Id[rVariable];

   Key::Input key(rDate, rInit, iOffset, locationId, rMemberId, variableId);

   // Check if time interpolation is needed
   // For missing offsets, find nearby offsets and interpolate
   // Don't do this for observations because this may not be valid for verification purposes
   if(mAllowTimeInterpolation && mType != Input::typeObservation && !hasOffset(iOffset)) {
      float lowerOffset = Global::MV;
      float upperOffset = Global::MV;
      int nOffsets = (int) mOffsets.size();

      if(nOffsets == 0) {
         std::stringstream ss;
         ss << "Input " << mName << " has no offsets" << std::endl;
         Global::logger->write(ss.str(), Logger::error);
      }

      // Before first offset
      if(iOffset < mOffsets[0]) {
         value = Global::MV;
      }
      // Later than last offset
      else if(iOffset > mOffsets[nOffsets-1]) {
         value = Global::MV;
      }
      // In between offsets
      else {
         for(int i = 0; i < nOffsets-1; i++) {
            if(iOffset > mOffsets[i]) {
               lowerOffset = mOffsets[i];
               upperOffset = mOffsets[i+1];
            }
         }

         float lowerValue = getValue(rDate, rInit, lowerOffset, iLocationNum, rMemberId, rVariable);
         float upperValue = getValue(rDate, rInit, upperOffset, iLocationNum, rMemberId, rVariable);
         // Use whichever value(s) are valid
         if(!Global::isValid(lowerValue)) {
            value = upperValue;
         }
         else if(!Global::isValid(upperValue)) {
            value = lowerValue;
         }
         else {
            //value = (upperValue + lowerValue)/2;
            float ymin = lowerValue;
            float dy = upperValue - lowerValue;
            float dx = upperOffset - lowerOffset;
            // Inteprolation points are identical
            if(dx == 0) {
               value = lowerValue;
            }
            // Linear interpolation
            else {
               float f  = (iOffset-lowerOffset) / dx;
               value = lowerValue + dy*f;
            }
         }
         std::stringstream ss;
         ss << "Input.cpp: " << mName << " does not have offset " << iOffset
            << ": Interpolation between " << lowerOffset << " and " << upperOffset;
         Global::logger->write(ss.str(), Logger::warning);
      }
   }
   // Don't interpolate
   else {
      if(!hasOffset(iOffset)) {
         // No data available
         value = Global::MV;
         std::stringstream ss;
         ss << "Input.cpp: " << mName << " does not have offset " << iOffset;
         Global::logger->write(ss.str(), Logger::critical);
      }
      else {
         Key::Input key0 = key;
         if(mCacheOtherLocations) {
            key0.location = 0;
         }
         if(mCacheOtherOffsets) {
            key0.offset = 0;
         }
         if(mCacheOtherVariables) {
            key0.variable = 0;
         }
         if(mCacheOtherMembers) {
            key0.member = 0;
         }

         if(mCache.isCached(key0)) {
            int index = getCacheIndex(key);
            value = mCache.get(key0)[index];
            if(value == Global::NC) {
               // Value hasn't been retrieved yet
               value = getValueCore(key);

               // We need to warn about this, because it probably slows down data retrival
               if(1 || !mHasWarnedCacheMiss) {
                  std::stringstream ss;
                  ss << "Input " << mName << " does not cache all values and must be re-read. This may be slow."
                     << " (" << key << ")";
                  Global::logger->write(ss.str(), Logger::warning);
                  mHasWarnedCacheMiss = true;
               }
               value = Global::MV;
            }
         }
         else {
            value = getValueCore(key);
         }
      }
   }

   assert(value != Global::NC);
   if(Global::isValid(value)) {
      bool isAltered = false; // Has the value been changed by calibration or QC?

      // Calibrate value
      float scale = mVariableScale[rVariable];
      float offset = mVariableOffset[rVariable];
      value = offset + scale*value;
      assert(!std::isnan(value));
      assert(!std::isinf(value));
      if(offset != 0 || scale != 1)
         isAltered = true;

      // Quality control
      if(mDoQc) {
         if(Global::isValid(mVariableMin[rVariable]) && Global::isValid(mVariableMax[rVariable])) {
            if(value < mVariableMin[rVariable] || value > mVariableMax[rVariable]) {
               std::stringstream ss;
               ss << rVariable << " value of " << value << " from " << mName << " on " << rDate << " " << iOffset << " assumed missing";
               Global::logger->write(ss.str(), Logger::warning);
               value = Global::MV;
               isAltered = true;
            }
         }
      }

      // Cache the change value
      // Can't cache, because then offset and scale is repeated each time
      // the value is accessed
      //if(isAltered)
      //   addToCache(key, value);
   }
   return value;
}

void Input::getValues(int rDate,
      int rInit,
      float iOffset,
      int rLocationId,
      std::string rVariable,
      std::vector<float>& iValues) const {
   for(int i = 0; i < (int) mMembers.size(); i++) {
      float value = getValue(rDate, rInit, iOffset, rLocationId, mMembers[i].getId(), rVariable);
      iValues.push_back(value);
   }
}
void Input::getValues(int rDate,
      int rInit,
      float iOffset,
      int rLocationId,
      std::string rVariable,
      Ensemble& iEnsemble) const {
   std::vector<float> values;
   getValues(rDate, rInit, iOffset, rLocationId, rVariable, values);

   iEnsemble.setValues(values);
   iEnsemble.setVariable(rVariable);
}

void Input::getAllValues(int iDate, int iInit, const std::string& iVariable, std::vector<Obs>& iObs) const {
   for(int i = 0; i < (int) mOffsets.size(); i++) {
      float offset = mOffsets[i];
      for(int l = 0; l < (int) mLocations.size(); l++) {
         Location location = mLocations[l];
         int memberId = 0; // TODO
         float value = getValue(iDate, iInit, offset, location.getId(), memberId, iVariable);
         Obs obs(value, iDate, offset, iVariable, location);
         iObs.push_back(obs);
      }
   }
}
void Input::getSurroundingLocations(const Location& iTarget, std::vector<Location>& iLocations, int iNumPoints) const {
   // Most often only the nearest point is needed. Therefore make a special cache for these
   if(iNumPoints == 1) {
      int nearestId = Global::MV;
      if(mCacheNearestLocation.isCached(iTarget)) {
         const std::vector<int> nearestIds = mCacheNearestLocation.get(iTarget);
         nearestId  = nearestIds[0];
      }
      else {
         float minDistance = Global::MV;
         for(int i = 0; i < (int) mLocations.size(); i++) {
            float distance = mLocations[i].getDistance(iTarget);
            if(Global::isValid(distance) && (minDistance == Global::MV || distance < minDistance)) {
               minDistance = distance;
               nearestId = i;
            }
         }
         assert(Global::isValid(nearestId));
         std::vector<int> nearestIds;
         nearestIds.push_back(nearestId);
         mCacheNearestLocation.add(iTarget, nearestIds);
      }
      // Return value
      iLocations.push_back(mLocations[nearestId]);
   }
   // General case where 'N' nearest neighbours are needed
   // This is slow when the number of input locations is large
   else {
      std::vector<int> allLocations;
      if(mCacheSurroundingLocations.isCached(iTarget)) {
         allLocations = mCacheSurroundingLocations.get(iTarget);
      }
      else {
         std::vector<std::pair<int, float> > distances;
         for(int i = 0; i < (int) mLocations.size(); i++) {
            float distance = mLocations[i].getDistance(iTarget);
            std::pair<int, float> p(i, distance);
            distances.push_back(p);
         }
         // TODO
         std::sort(distances.begin(), distances.end(), Global::sort_pair_second<int, float>());
         for(int i = 0; i < (int) distances.size(); i++) {
            int index = distances[i].first;
            allLocations.push_back(index);
         }
         mCacheSurroundingLocations.add(iTarget, allLocations);
      }

      // Only return iNumPoints locations
      for(int i = 0; i < iNumPoints; i++) {
         if(i < (int) allLocations.size()) {
            iLocations.push_back(mLocations[allLocations[i]]);
         }
      }
   }
}

void Input::getSurroundingLocationsByRadius(const Location& rTarget, std::vector<Location>& rLocations, float iRadius) const {
   std::vector<std::pair<int, float> > distances;
   for(int i = 0; i < (int) mLocations.size(); i++) {
      float distance = mLocations[i].getDistance(rTarget);
      if(distance <= iRadius) {
         rLocations.push_back(mLocations[i]);
      }
   }
}
void Input::getDates(std::vector<int>& rDates) const {
   if(mIsDatesCached) {
      rDates = mDates;
      return;
   }

   if (!boost::filesystem::exists(mDataDirectory)) {
      Global::logger->write("No input dates available", Logger::message);
      return;
   }

   if(getDatesCore(rDates)) {
      sort(rDates.begin(), rDates.end());
      mDates = rDates;
      mIsDatesCached = true;
   }
   else {
      Global::logger->write("Could not get dates", Logger::message);
   }
   return;
}

bool Input::getDatesCore(std::vector<int>& iDates) const {
   boost::filesystem::directory_iterator end_itr; // default construction yields past-the-end
   for(boost::filesystem::directory_iterator itr(mDataDirectory); itr != end_itr; ++itr) {
      if(1 || !boost::filesystem::is_directory(itr->status())) {
         std::string filename = boost::filesystem::basename(itr->path().string());
         size_t pos2 = filename.find("_");
         if((int) filename.size() == mFilenameDateStartIndex + 8 || pos2 == mFilenameDateStartIndex + 8) {
            std::stringstream ssi;
            ssi << filename.substr(mFilenameDateStartIndex,8);
            int date;
            ssi >> date;
            std::vector<int>::iterator it = find(iDates.begin(), iDates.end(), date);
            if(it == iDates.end()) {
               iDates.push_back(date);
            }
         }
      }
   }
   return true;
}

std::string Input::getName() const {
   return mName;
}
std::string Input::getFolder() const {
   return mFolder;
}

int Input::getOffsetIndex(float iOffset) const {
   std::map<float, int>::const_iterator it = mOffsetMap.find(iOffset);
   if(it != mOffsetMap.end()) {
      return it->second;
   }
   else {
      return (int) Global::MV;
      //std::cout << "Offset " << iOffset << " does not exist for " << mName << std::endl;
      //assert(0);
   }
}

int Input::getNearestOffsetIndex(float iOffset) const {
   int index = getOffsetIndex(iOffset);
   if(!Global::isValid(index)) {
      float diff = Global::MV;
      std::map<float, int>::const_iterator it;
      for(it = mOffsetMap.begin(); it != mOffsetMap.end(); it++) {
         float currDiff = fabs(iOffset - it->first);
         if(!Global::isValid(diff) || (currDiff < diff)) {
            index = it->second;
            diff = currDiff;
         }
      }
   }
   return index;
}
void Input::getMembers(std::vector<Member>& rMembers) const {
   rMembers = mMembers;
}
void Input::getVariables(std::vector<std::string>& iVariables) const {
   std::map<std::string, int>::const_iterator it;
   for(it = mVariableMap.begin(); it != mVariableMap.end(); it++) {
      iVariables.push_back(it->first);
   }
}
void Input::getLocations(std::vector<Location>& iLocations) const {
   iLocations = mLocations;
}
void Input::getOffsets(std::vector<float>& iOffsets) const {
   iOffsets = mOffsets;
}
void Input::makeOffsetMap() const {
   for(int i = 0 ; i < (int) mOffsets.size(); i++) {
      //std::cout << "Adding offset[" << mOffsets[i] << "] = " << i << std::endl;
      mOffsetMap[mOffsets[i]] = i;
   }
}
void Input::makeLocationMap() const {
   for(int i = 0 ; i < (int) mLocations.size(); i++) {
      //std::cout << "Adding offset[" << mOffsets[i] << "] = " << i << std::endl;
      mLocationMap[mLocations[i].getId()] = i;
   }
}

float Input::getCacheSize() const {
   return Global::MV;//mCache.size()*getBytesPerCacheEntry();
}

float Input::getMaxCacheSize() const {
   int maxSize = Global::MV;//mCache.maxSize();
   if(Global::isMissing(maxSize))
      return Global::MV;
   else
      return Global::MV;//mCache.maxSize()*getBytesPerCacheEntry();
}

void Input::setCacheOtherOffsets(bool iStatus) {
   mCacheOtherOffsets = iStatus;
}
void Input::setCacheOtherLocations(bool iStatus) {
   mCacheOtherLocations = iStatus;
}
void Input::setCacheOtherMembers(bool iStatus) {
   mCacheOtherMembers = iStatus;
}

void Input::loadVariables() const {
   std::stringstream ss0;
   ss0 << Input::getInputDirectory() << mFolder << "/variables.nl";
   Namelist nl(ss0.str());
   std::vector<std::string> keys;
   nl.getAllKeys(keys);

   for(int i = 0; i < (int) keys.size(); i++) {
      std::string key = keys[i];
      Options opt(nl.findLine(key));
      std::string value;
      opt.getValue("name", value);
      mVariableRename[key] = value;
      mVariableMap[key] = i;

      mVariable2Id[key] = i;
      mId2Variable[i] = key;
      mLocalVariable2Id[value] = i;
      mId2LocalVariable[i] = value;

      float scale  = 1;
      float offset = 0;
      opt.getValue("scale", scale);
      opt.getValue("offset", offset);

      mVariableScale[key] = scale;
      mVariableOffset[key] = offset;
      assert(Global::isValid(scale));
      assert(Global::isValid(offset));

      const Variable* var = Variable::get(key);
      mVariableMin[key] = var->getMin();
      mVariableMax[key] = var->getMax();
      mVariables.push_back(key);
   }
}

Input::Type Input::getType() const {
   return mType;
}

std::string Input::getTypeDescription(Input::Type iType) {
   if(iType == typeForecast) 
      return "forecast";
   else if(iType == typeObservation) 
      return "observation";
   else
      return "any";
}
void Input::setMaxCacheSize(float iMaxCacheSize) {
   std::cout << "Setting max size: " << iMaxCacheSize << std::endl;
   // don't set this, should be set through options
   assert(0);
   mMaxCacheSize = iMaxCacheSize;
}
std::string Input::getFileExtension() const {
   std::stringstream ss;
   if(mFileExtension != "") {
      ss << ".";
   }
   ss << mFileExtension;
   return ss.str();
}


void Input::loadLocations() const {
   std::stringstream ss0;
   ss0 << Input::getInputDirectory() << mFolder << "/locations.nl";
   Namelist nl(ss0.str());
   std::vector<std::vector<float> > values;
   nl.getAllValues<float>(values);

   for(int i = 0; i < (int) values.size(); i++) {
      int   id   = (int) values[i][0];

      std::stringstream ss;
      ss << id;

      Options opt(nl.findLine(ss.str()));
      float lat  = Global::MV;
      float lon  = Global::MV;
      float elev = Global::MV;
      std::string code="";
      std::string name="";
      opt.getRequiredValue("lat", lat); 
      opt.getRequiredValue("lon", lon); 
      opt.getValue("elev", elev); 
      opt.getValue("code", code); 
      opt.getValue("name", name); 

      Location loc(mName, id, lat, lon, elev, name);
      if(code.size()) {
         loc.setCode(code);
      }
      mLocations.push_back(loc);
   }
}

void Input::loadOffsets() const {
   std::stringstream ss0;
   ss0 << Input::getInputDirectory() << mFolder << "/offsets.nl";
   Namelist nl(ss0.str());
   nl.getAllKeys(mOffsets);
}


void Input::loadMembers() const {
   // Members
   std::stringstream ss0;
   ss0 << Input::getInputDirectory() << mFolder << "/members.nl";
   Namelist nlMembers(ss0.str());
   std::vector<int> keys;
   nlMembers.getAllKeys(keys);
   for(int i = 0; i < (int) keys.size(); i++) {
      int key = keys[i];
      std::stringstream ss;
      ss << key;
      Options options(nlMembers.findLine(ss.str()));
      std::string model = "";
      options.getValue("model", model);
      float res = Global::MV;
      options.getValue("resolution", res);
      int id = key;
      Member member(mName, res, model, id);
      mMembers.push_back(member);
   }
}

float Input::getValueCore(const Key::Input& key) const {
   return getValueCore(key.date, key.init, key.offset, key.location, key.member, mId2Variable[key.variable]);
}
float Input::getValueCore(int rDate, int rInit, float iOffset, int rLocationId, int rMemberId, std::string rVariable) const {
   assert(0);
}

void Input::addToCache(const Key::Input& iKey, float iValue) const {
   // Use vector
   Key::Input key0 = iKey;
   if(mCacheOtherLocations) {
      key0.location = 0;
   }
   if(mCacheOtherOffsets) {
      key0.offset = 0;
   }
   if(mCacheOtherVariables) {
      key0.variable = 0;
   }
   if(mCacheOtherMembers) {
      key0.member = 0;
   }
   int index = getCacheIndex(iKey);
   int vecSize = getCacheVectorSize();

   if(!(key0 == mLastCachedKey)) {
      // We haven't recently accessed the cache for this key
      if(mCache.isCached(key0)) {
         std::map<Key::Input, std::vector<float> >::iterator it = mCache.get(key0, 0);
         mLastCachedVector = &it->second;
      }
      else {
         std::vector<float> values;
         values.resize(vecSize, Global::NC);
         mCache.add(key0, values);
         std::map<Key::Input, std::vector<float> >::iterator it = mCache.get(key0, 0);
         mLastCachedVector = &it->second;
      }
      mLastCachedKey = key0;
   }
   assert((int) mLastCachedVector->size() == vecSize);
   assert(index >= 0 && index < vecSize);
   (*mLastCachedVector)[index] = iValue;
}
void Input::optimizeCacheOptions() {
   std::stringstream ss;
   ss << "Input " << mName << " does not specify optimized cache options";
   Global::logger->write(ss.str(), Logger::warning);
}

void Input::write(const Input& iData, const Input& iDimensions, int iDate) const {
   writeCore(iData, iDimensions, iDate);
}
void Input::write(const Input& iInput, int iDate) const {
   writeCore(iInput, iInput, iDate);
}

void Input::writeCore(const Input& iData, const Input& iDimensions, int iDate) const {
   std::stringstream ss;
   ss << "Input " << mName << " cannot write files because its type has not been implemented";
   Global::logger->write(ss.str(), Logger::error);
}

int Input::getCacheVectorSize() const {
   int vecSize = 1;
   if(mCacheOtherLocations) {
      vecSize *= mLocations.size();
   }
   if(mCacheOtherOffsets) {
      vecSize *= mOffsets.size();
   }
   if(mCacheOtherVariables) {
      vecSize *= mVariables.size();
   }
   if(mCacheOtherMembers) {
      vecSize *= mMembers.size();
   }
   return vecSize;
}

int Input::getCacheIndex(const Key::Input& iKey) const {
   int index = 0;
   int currStep = 1;
   if(mCacheOtherLocations) {
      index += iKey.location * currStep;
      currStep *= mLocations.size();
   }
   if(mCacheOtherOffsets) {
      int offsetIndex = mOffsetMap[iKey.offset];
      index += offsetIndex * currStep;
      currStep *= mOffsets.size();
   }
   if(mCacheOtherVariables) {
      index += iKey.variable * currStep;
      currStep *= mVariables.size();
   }
   if(mCacheOtherMembers) {
      index += iKey.member * currStep;
   }
   assert(index >= 0 && index < getCacheVectorSize());
   return index;
}

void Input::invalidCacheOptions() const {
   std::stringstream ss;
   ss << "Caching options for " << mName << " are invalid:" << std::endl;
   ss << "   other variables: " << mCacheOtherVariables << std::endl;
   ss << "   other locations: " << mCacheOtherLocations << std::endl;
   ss << "   other offsets:   " << mCacheOtherOffsets   << std::endl;
   ss << "   other members:   " << mCacheOtherMembers   << std::endl;
   Global::logger->write(ss.str(), Logger::warning);
}

std::string Input::getSampleFilename() const {
   // Check if sample file exists
   std::string filename = getConfigFilename("sample");
   std::ifstream ifs(filename.c_str(), std::ifstream::in);
   if(ifs) {
      ifs.close();
      return filename;
   }

   return getSampleFilenameCore();
}

std::string Input::getSampleFilenameCore() const {
   boost::filesystem::directory_iterator end_itr; // default construction yields past-the-end
   if(!boost::filesystem::exists(mDataDirectory)) {
      std::stringstream ss;
      ss << "Input: Data directory " << mDataDirectory << " does not exist for " << mName;;
      Global::logger->write(ss.str(), Logger::error);
   }
   boost::filesystem::directory_iterator itr(mDataDirectory);
   std::string dataFilename;
   for(boost::filesystem::directory_iterator itr(mDataDirectory); itr != end_itr; ++itr) {
      if(!boost::filesystem::is_directory(itr->status())) {
         dataFilename = (itr->path().string());
         break;
      }
   }
   if(dataFilename == "") {
      std::stringstream ss;
      ss << "Input: Could not find a suitable file to use as sample file for dataset " << mName
         << ". Perhaps no data is downloaded?";
      Global::logger->write(ss.str(), Logger::error);
      return "";
   }

   std::string filename = getConfigFilename("sample");
   std::stringstream ss;
   ss << "Input: Could not find sample file: " << filename << ". "
      << "Using " << dataFilename << " as the sample file instead.";
   Global::logger->write(ss.str(), Logger::warning);
   return dataFilename;
}
