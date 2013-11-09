#include "SchemesHeader.inc"
#include "Input.h"
#include "../Variables/Variable.h"
#include "../Options.h"
#include "../Location.h"
#include "../Member.h"
#include "../Loggers/Logger.h"
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
      mAllowTimeInterpolation(false),
      mHasInit(false),
      mHasWarnedCacheMiss(false),
      mOptimizeCache(false),
      mIsDatesCached(false),
      mNumLocations(Global::MV),
      mNumOffsets(Global::MV),
      mNumMembers(Global::MV),
      mNumVariables(Global::MV),
      mForceLimits(false),
      mFileExtension(""),
      mFilenameDateStartIndex(0) {
   // Process options
   iOptions.getRequiredValue("tag", mName);
   //! Name of folder where namelists resides in ./input/
   iOptions.getRequiredValue("folder", mFolder);
   //! Specify file extension used on data files. Uses default otherwise.
   iOptions.getValue("fileExtension", mFileExtension);
   //! Should this dataset allow values to be interpolated for times between its offsets?
   iOptions.getValue("allowTimeInterpolation", mAllowTimeInterpolation);
   //! When a value is retrived, should all other variables at the same location/member/offset be
   //! cached? This is advantageous if one file stores several variables, and there is an overhead
   //! in making multiple calls to getting data.
   iOptions.getValue("cacheOtherVariables", mCacheOtherVariables);
   iOptions.getValue("cacheOtherLocations", mCacheOtherLocations);
   iOptions.getValue("cacheOtherMembers", mCacheOtherMembers);
   iOptions.getValue("cacheOtherOffsets", mCacheOtherOffsets);
   //! Should the dataset figure out how to optimize the cache options itself?
   iOptions.getValue("optimize", mOptimizeCache);
   //! Should the dataset round values up/down to the boundary if it is outside?
   iOptions.getValue("forceLimits", mForceLimits);

   // Type
   std::string type;
   //! Dataset type. One of 'forecast' or 'observation'.
   iOptions.getRequiredValue("type", type);
   if(type == "observation")
      mType = typeObservation;
   else if(type == "forecast")
      mType = typeForecast;
   else
      Global::logger->write("Unrecognized input type", Logger::error);
   // Directories
   std::stringstream ss;
   ss << getInputDirectory() << mFolder << "/";
   mDirectory  = ss.str();

   //! Full path of where data files are located
   if(!iOptions.getValue("dataDir", mDataDirectory)) {
      ss << "data/";
      mDataDirectory = ss.str();
   }
   /////////////
   // Caching //
   /////////////
   //! Only allow these location IDs
   iOptions.getValues("locations", mAllowLocations);

   std::stringstream ss0;
   ss0 << getName() << ":Loc";
   mCacheSurroundingLocations.setName(ss0.str());
   mCacheNearestLocation.setName(ss0.str());

   // Data cache
   mCache.setName(getName());
   float maxCacheSize = Global::MV;
   //! Limit the data cache size to this many bytes
   if(iOptions.getValue("maxCacheSize", maxCacheSize)) {
      mCache.setMaxSize(maxCacheSize);
   }
}
void Input::init() {
   assert(!mHasInit);

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

std::string Input::getNamelistFilename(std::string iNamelist) const {
   std::stringstream ss;
   ss << getDirectory() << iNamelist << ".nl";
   return ss.str();
}
bool Input::hasVariable(std::string iVariableName) const {
   int id;
   return getVariableIdFromVariable(iVariableName, id);
}

bool Input::hasOffset(float iOffset) const {
   return Global::isValid(getOffsetIndex(iOffset));
}

// NOTE: Any recursive calls to getValue in here should get the raw values, since they get
// calibrated at the end anyway
float Input::getValue(int iDate, int iInit, float iOffset, int iLocationNum, int iMemberId, std::string iVariable, bool iCalibrate) const {
   // Check that inputs make sense
   int locationIndex = getLocationIndex(iLocationNum);

   // Since obs from the same valid times (but different dates/offsets), we can try to find
   // another offset from a different day if the desired offset doesn't exist
   std::vector<float> offsets = getOffsets();
   if(getType() == Input::typeObservation && !Global::isValid(getOffsetIndex(iOffset))) {
      for(int i = 0; i < offsets.size(); i++) {
         if(abs(offsets[i] - iOffset) % 24 == 0) {
            iDate = Global::getDate(iDate, iInit, iOffset - offsets[i]);
            iOffset =  offsets[i];
         }
      }
   }
   /*
   if(getType() == Input::typeObservation && iOffset >= 24) {
      iDate = Global::getDate(iDate, iInit, iOffset);
      iOffset =  fmod(iOffset, 24);
   }
   else if(getType() == Input::typeObservation && iOffset < 0) {
      iDate = Global::getDate(iDate, iInit, iOffset);
      iOffset = Global::getOffset(iDate, iOffset);
   }
  */

   float value = Global::MV;
   int variableId;
   bool found = getVariableIdFromVariable(iVariable, variableId);
   assert(found);

   Key::Input key(iDate, iInit, iOffset, locationIndex, iMemberId, variableId);

   // Check if time interpolation is needed
   // For missing offsets, find nearby offsets and interpolate
   // Don't do this for observations because this may not be valid for verification purposes
   if(mAllowTimeInterpolation && getType() != Input::typeObservation && !hasOffset(iOffset)) {
      float lowerOffset = Global::MV;
      float upperOffset = Global::MV;
      int nOffsets = (int) offsets.size();

      if(nOffsets == 0) {
         std::stringstream ss;
         ss << "Input " << getName() << " has no offsets" << std::endl;
         Global::logger->write(ss.str(), Logger::error);
      }

      // Before first offset
      if(iOffset < offsets[0]) {
         value = Global::MV;
      }
      // Later than last offset
      else if(iOffset > offsets[nOffsets-1]) {
         value = Global::MV;
      }
      // In between offsets
      else {
         for(int i = 0; i < nOffsets-1; i++) {
            if(iOffset > offsets[i]) {
               lowerOffset = offsets[i];
               upperOffset = offsets[i+1];
            }
         }

         float lowerValue = getValue(iDate, iInit, lowerOffset, iLocationNum, iMemberId, iVariable, false);
         float upperValue = getValue(iDate, iInit, upperOffset, iLocationNum, iMemberId, iVariable, false);
         // Use whichever value(s) are valid
         if(!Global::isValid(lowerValue)) {
            value = upperValue;
         }
         else if(!Global::isValid(upperValue)) {
            value = lowerValue;
         }
         else {
            //value = (upperValue + lowerValue)/2;
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
         ss << "Input.cpp: " << getName() << " does not have offset " << iOffset
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
         ss << "Input.cpp: " << getName() << " does not have offset " << iOffset;
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
                  ss << "Input " << getName() << " does not cache all values and must be re-read. This may be slow."
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

   if(!iCalibrate)
      return(value);

   // Calibrate
   assert(value != Global::NC);
   if(Global::isValid(value)) {
      bool isAltered = false; // Has the value been changed by calibration or QC?

      // Calibrate value
      float scale = getVariableScale(iVariable);
      float offset = getVariableOffset(iVariable);
      value = offset + scale*value;
      assert(!std::isnan(value));
      assert(!std::isinf(value));
      if(offset != 0 || scale != 1)
         isAltered = true;

      // Cache the change value
      // Can't cache, because then offset and scale is repeated each time
      // the value is accessed
      // ???? Probably ok, it just means that calibration has to be done everytime the data is
      // retrieved from cache
      //if(isAltered)
      //   addToCache(key, value);
   }

   if(mForceLimits && Global::isValid(value)) {
      const Variable* var = Variable::get(iVariable);
      if(Global::isValid(var->getMin()) && value < var->getMin()) {
         value = var->getMin();
      }
      else if(Global::isValid(var->getMax()) && value > var->getMax()) {
         value = var->getMax();
      }
   }
   return value;
}

void Input::getValues(int iDate,
      int iInit,
      float iOffset,
      int iLocationId,
      std::string iVariable,
      std::vector<float>& iValues) const {
   for(int i = 0; i < (int) mMembers.size(); i++) {
      float value = getValue(iDate, iInit, iOffset, iLocationId, mMembers[i].getId(), iVariable);
      iValues.push_back(value);
   }
}
void Input::getValues(int iDate,
      int iInit,
      float iOffset,
      int iLocationId,
      std::string iVariable,
      Ensemble& iEnsemble) const {
   std::vector<float> values;
   getValues(iDate, iInit, iOffset, iLocationId, iVariable, values);

   iEnsemble.setValues(values);
   iEnsemble.setVariable(iVariable);
}

void Input::getSurroundingLocations(const Location& iTarget, std::vector<Location>& iLocations, int iNumPoints) const {
   // Most often only the nearest point is needed. Therefore make a special cache for these
   std::vector<Location> locations = getLocations();
   if(iNumPoints == 1) {
      int nearestId = Global::MV;
      if(mCacheNearestLocation.isCached(iTarget)) {
         const std::vector<int> nearestIds = mCacheNearestLocation.get(iTarget);
         nearestId  = nearestIds[0];
      }
      else {
         float minDistance = Global::MV;
         for(int i = 0; i < (int) locations.size(); i++) {
            float distance = locations[i].getDistance(iTarget);
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
      iLocations.push_back(locations[nearestId]);
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
         for(int i = 0; i < (int) locations.size(); i++) {
            float distance = locations[i].getDistance(iTarget);
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
            iLocations.push_back(locations[allLocations[i]]);
         }
      }
   }
}

void Input::getSurroundingLocationsByRadius(const Location& iTarget, std::vector<Location>& iLocations, float iRadius) const {
   std::vector<std::pair<int, float> > distances;
   std::vector<Location> locations = getLocations();
   for(int i = 0; i < (int) locations.size(); i++) {
      float distance = locations[i].getDistance(iTarget);
      if(distance <= iRadius) {
         iLocations.push_back(locations[i]);
      }
   }
}
void Input::getDates(std::vector<int>& iDates) const {
   if(mIsDatesCached) {
      iDates = mDates;
      return;
   }

   if(getDatesCore(iDates)) {
      sort(iDates.begin(), iDates.end());
      mDates = iDates;
      mIsDatesCached = true;
   }
   else {
      Global::logger->write("Could not get dates", Logger::message);
   }
   return;
}

bool Input::getDatesCore(std::vector<int>& iDates) const {
   if (!boost::filesystem::exists(getDataDirectory())) {
      std::stringstream ss;
      ss << "Data directory does not exist for " << getName();
      Global::logger->write(ss.str(), Logger::message);
      return false;
   }

   boost::filesystem::directory_iterator end_itr; // default construction yields past-the-end
   for(boost::filesystem::directory_iterator itr(getDataDirectory()); itr != end_itr; ++itr) {
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
std::string Input::getDirectory() const {
   return mDirectory;
}

int Input::getOffsetIndex(float iOffset) const {
   if(mOffsetMap.size() == 0) {
      // Load offset map
      std::vector<float> offsets = getOffsets();
      for(int i = 0 ; i < (int) offsets.size(); i++) {
         mOffsetMap[mOffsets[i]] = i;
      }
   }

   std::map<float, int>::const_iterator it = mOffsetMap.find(iOffset);

   if(it != mOffsetMap.end()) {
      return it->second;
   }
   else {
      return (int) Global::MV;
   }
}

int Input::getNearestOffsetIndex(float iOffset) const {
   int index = getOffsetIndex(iOffset);
   if(!Global::isValid(index)) {
      float diff = Global::MV;
      std::map<float, int>::const_iterator it;
      std::vector<float> offsets = getOffsets();
      for(int i = 0; i < offsets.size(); i++) {
         float offset = offsets[i];
         float currDiff = fabs(iOffset - offset);
         if(!Global::isValid(diff) || (currDiff < diff)) {
            index = i;
            diff = currDiff;
         }
      }
   }
   return index;
}
std::vector<Member> Input::getMembers() const {
   if(mMembers.size() == 0) {
      getMembersCore(mMembers);
      // Check that obs dataset only has one member
      if(getType() == Input::typeObservation) {
         if(mMembers.size() != 1) {
            std::stringstream ss;
            ss << "Input " << getName() << " has " << mMembers.size() << " members. Must be 1";
            Global::logger->write(ss.str(), Logger::error);
         }
      }
   }
   return mMembers;
}
std::vector<std::string> Input::getVariables() const {
   if(mVariables.size() == 0) {
      // Load variables and cache them
      std::stringstream ss0;
      std::string filename = getNamelistFilename("variables");
      Namelist nl(filename);
      std::vector<std::string> keys;
      nl.getAllKeys(keys);

      mId2Variable.resize(keys.size());
      mId2LocalVariable.resize(keys.size());
      for(int i = 0; i < (int) keys.size(); i++) {
         std::string key = keys[i];
         if(key != "") {
            Options opt(nl.findLine(key));
            std::string value;
            opt.getValue("name", value);
            mVariable2LocalVariable[key] = value;
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

            mVariables.push_back(key);
         }
      }
      assert(mVariables.size() > 0);
   }
   return mVariables;
}
std::vector<Location> Input::getLocations() const {
   if(mLocations.size() == 0) {
      getLocationsCore(mLocations);
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
      assert(mLocations.size() > 0);
   }
   return mLocations;
}
std::vector<float> Input::getOffsets() const {
   if(mOffsets.size() == 0) {
      getOffsetsCore(mOffsets);
      assert(mOffsets.size() > 0);
   }
   return mOffsets;
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
std::string Input::getFileExtension() const {
   std::string fileExtension = mFileExtension;
   if(fileExtension == "")
      fileExtension = getDefaultFileExtension();
   std::stringstream ss;
   if(fileExtension != "") {
      ss << ".";
   }
   ss << fileExtension;
   return ss.str();
}


void Input::getLocationsCore(std::vector<Location>& iLocations) const {
   std::string filename = getNamelistFilename("locations");
   Namelist nl(filename);
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

      Location loc(getName(), id, lat, lon, elev, name);
      if(code.size()) {
         loc.setCode(code);
      }
      iLocations.push_back(loc);
   }
}

void Input::getOffsetsCore(std::vector<float>& iOffsets) const {
   std::string filename = getNamelistFilename("offsets");
   Namelist nl(filename);
   nl.getAllKeys(iOffsets);
}


void Input::getMembersCore(std::vector<Member>& iMembers) const {
   // Members
   std::string filename = getNamelistFilename("members");
   Namelist nlMembers(filename);
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
      Member member(getName(), res, model, id);
      iMembers.push_back(member);
   }
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
   ss << "Input " << getName() << " does not specify optimized cache options";
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
   ss << "Input " << getName() << " cannot write files because its type has not been implemented";
   Global::logger->write(ss.str(), Logger::error);
}

int Input::getCacheVectorSize() const {
   int vecSize = 1;
   if(mCacheOtherLocations) {
      vecSize *= getNumLocations();
   }
   if(mCacheOtherOffsets) {
      vecSize *= getNumOffsets();
   }
   if(mCacheOtherVariables) {
      vecSize *= getNumVariables();
   }
   if(mCacheOtherMembers) {
      vecSize *= getNumMembers();
   }
   return vecSize;
}

int Input::getCacheIndex(const Key::Input& iKey) const {
   int index = 0;
   int currStep = 1;
   if(mCacheOtherLocations) {
      index += iKey.location * currStep;
      currStep *= getNumLocations();
   }
   if(mCacheOtherOffsets) {
      int offsetIndex = getOffsetIndex(iKey.offset);
      index += offsetIndex * currStep;
      currStep *= getNumOffsets();
   }
   if(mCacheOtherVariables) {
      index += iKey.variable * currStep;
      currStep *= getNumVariables();
   }
   if(mCacheOtherMembers) {
      index += iKey.member * currStep;
   }
   assert(index >= 0 && index < getCacheVectorSize());
   return index;
}

void Input::notifyInvalidCacheOptions() const {
   std::stringstream ss;
   ss << "Caching options for " << getName() << " are invalid:" << std::endl;
   ss << "   other variables: " << mCacheOtherVariables << std::endl;
   ss << "   other locations: " << mCacheOtherLocations << std::endl;
   ss << "   other offsets:   " << mCacheOtherOffsets   << std::endl;
   ss << "   other members:   " << mCacheOtherMembers   << std::endl;
   Global::logger->write(ss.str(), Logger::warning);
}

std::string Input::getSampleFilename() const {
   // Check if sample file exists, and use this
   std::stringstream ss;
   ss << getDirectory() << "sample" << getFileExtension();
   std::string filename = ss.str();
   std::ifstream ifs(filename.c_str(), std::ifstream::in);
   if(ifs) {
      ifs.close();
      return filename;
   }
   std::stringstream ss1;
   ss1 << "Input: Sample file " << filename << " does not exist. Searching for a suitable in data directory";
   Global::logger->write(ss1.str(), Logger::warning);

   // Otherwise try to search for one in the data directory
   return getSampleFilenameCore();
}

std::string Input::getSampleFilenameCore() const {
   boost::filesystem::directory_iterator end_itr; // default construction yields past-the-end
   std::string dataDirectory = getDataDirectory();
   if(!boost::filesystem::exists(dataDirectory)) {
      std::stringstream ss;
      ss << "Input: Data directory " << dataDirectory << " does not exist for " << getName();
      Global::logger->write(ss.str(), Logger::error);
   }
   boost::filesystem::directory_iterator itr(dataDirectory);
   std::string dataFilename;
   for(boost::filesystem::directory_iterator itr(dataDirectory); itr != end_itr; ++itr) {
      if(!boost::filesystem::is_directory(itr->status())) {
         dataFilename = (itr->path().string());
         break;
      }
   }
   if(dataFilename == "") {
      std::stringstream ss;
      ss << "Input: Could not find a suitable file to use as sample file for dataset " << getName()
         << ". Perhaps no data is downloaded?";
      Global::logger->write(ss.str(), Logger::error);
      return "";
   }
   else {
      std::stringstream ss;
      ss << "Input: Using " << dataFilename << " as the sample file instead.";
      Global::logger->write(ss.str(), Logger::warning);
      return dataFilename;
   }
}

int Input::getLocationIndex(float iLocationId) const {
   if(mLocationMap.size() == 0) {
      // Load location map
      std::vector<Location> locations = getLocations();
      for(int i = 0 ; i < (int) mLocations.size(); i++) {
         mLocationMap[mLocations[i].getId()] = i;
      }
   }

   std::map<int, int>::const_iterator it = mLocationMap.find(iLocationId);

   if(it != mLocationMap.end()) {
      return it->second;
   }
   else {
      return (int) Global::MV;
   }
}

float Input::getVariableOffset(const std::string& iVariable) const {
   if(mVariableOffset.size() == 0)
      getVariables();
   return mVariableOffset[iVariable];
}
float Input::getVariableScale(const std::string& iVariable) const {
   if(mVariableScale.size() == 0)
      getVariables();
   return mVariableScale[iVariable];
}

/*
std::string Input::getLocalVariableName(std::string iVariable) const {
   if(mVariable2LocalVariable.size() == 0)
      getVariables();
   return mVariable2LocalVariable[iVariable];
}
std::string Input::getLocalVariableName(int iVariableId) const {
   if(mId2LocalVariable.size() == 0)
      getvariables();
   return mId2LocalVariable[iVariableId];
}
std::string Input::getVariableName(std::string iLocalVariable) const {
   if(mLocalVariable2Variable.size() == 0)
      getVariables();
   return mLocalVariable2Variable[iLocalVariable];
}
std::string Input::getVariableName(int iVariableId) const {
   if(mId2Variable.size() == 0)
      getVariables();
   return mId2Variable[iVariableId];
}
int Input::getVariableIdFromVariable(std::string iVariable) const {
   if(mVariable2Id.size() == 0)
      getvariables();
   return mVariable2Id[ivariable];
}
int Input::getVariableIdFromLocalVariable(std::string iLocalVariable) const {
   if(mLocalVariable2Variable.size() == 0)
      getvariables();
   return mLocalVariable2Variable[iLocalVariable];
}
*/

bool Input::getLocalVariableName(std::string iVariable, std::string& iLocalVariable) const {
   if(mVariable2LocalVariable.size() == 0)
      getVariables();
   std::map<std::string, std::string>::const_iterator it = mVariable2LocalVariable.find(iVariable);
   bool found = it != mVariable2LocalVariable.end();
   if(found)
      iLocalVariable = it->second;
   return found;
}
bool Input::getLocalVariableName(int iVariableId, std::string& iLocalVariable) const {
   if(mId2LocalVariable.size() == 0)
      getVariables();
   bool found = mId2LocalVariable.size() > iVariableId;
   if(found)
      iLocalVariable = mId2LocalVariable[iVariableId];
   return found;
}
bool Input::getVariableName(std::string iLocalVariable, std::string& iVariable) const {
   if(mLocalVariable2Variable.size() == 0)
      getVariables();
   std::map<std::string, std::string>::const_iterator it = mLocalVariable2Variable.find(iLocalVariable);
   bool found = it != mLocalVariable2Variable.end();
   if(found)
      iVariable = it->second;
   return found;
}
bool Input::getVariableName(int iVariableId, std::string& iVariable) const {
   if(mId2Variable.size() == 0)
      getVariables();
   bool found = mId2Variable.size() > iVariableId;
   if(found)
      iVariable = mId2Variable[iVariableId];
   return found;
}
bool Input::getVariableIdFromVariable(std::string iVariable, int& iVariableId) const {
   if(mVariable2Id.size() == 0)
      getVariables();
   std::map<std::string, int>::const_iterator it = mVariable2Id.find(iVariable);
   bool found = it != mVariable2Id.end();
   if(found)
      iVariableId = it->second;
   return found;
}
bool Input::getVariableIdFromLocalVariable(std::string iLocalVariable, int& iVariableId) const {
   if(mLocalVariable2Id.size() == 0)
      getVariables();
   std::map<std::string, int>::const_iterator it = mLocalVariable2Id.find(iLocalVariable);
   bool found = it != mLocalVariable2Id.end();
   if(found)
      iVariableId = it->second;
   return found;
}

std::string Input::getDataDirectory() const {
   return mDataDirectory;
}

std::string Input::getDefaultFileExtension() const {
   return "";
}

int Input::getNumLocations() const {
   if(!Global::isValid(mNumLocations))
      mNumLocations = getLocations().size();
   return(mNumLocations);
}
int Input::getNumOffsets() const {
   if(!Global::isValid(mNumOffsets))
      mNumOffsets = getOffsets().size();
   return(mNumOffsets);
}
int Input::getNumVariables() const {
   if(!Global::isValid(mNumVariables))
      mNumVariables = getVariables().size();
   return(mNumVariables);
}
int Input::getNumMembers() const {
   if(!Global::isValid(mNumMembers))
      mNumMembers = getMembers().size();
   return(mNumMembers);
}
