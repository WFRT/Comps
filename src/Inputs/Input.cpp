#include "SchemesHeader.inc"
#include "Input.h"
#include "../Variables/Variable.h"
#include "../Options.h"
#include "../Location.h"
#include "../Member.h"
#include "../Loggers/Logger.h"
#include "../Obs.h"
#include "../Ensemble.h"
#include <iomanip>
#include <iostream>
#include <boost/algorithm/string/replace.hpp>

/*********
 * Input *
 ********/
Input::Input(const Options& iOptions) : Component(iOptions),
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
      mNumInits(Global::MV),
      mNumMembers(Global::MV),
      mNumVariables(Global::MV),
      mLocationFilename(""),
      mForceLimits(false),
      mInitDelay(0),
      mLastOffset(Global::MV),
      mLastOffsetIndex(Global::MV),
      mStartDate(Global::MV),
      mEndDate(Global::MV),
      mMinLat(Global::MV),
      mMaxLat(Global::MV),
      mMinLon(Global::MV),
      mMaxLon(Global::MV),
      mReplaceMissing(false),
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

   //! If the locations are specified in a separate file, what is its filename?
   iOptions.getValue("locationFilename", mLocationFilename);

   //! How many hours does it take before forecasts are available after initialization?
   iOptions.getValue("initDelay", mInitDelay);

   iOptions.getValues("offsets", mOffsets);

   //! If a date/init is missing, should an older date/init be used?
   iOptions.getValue("replaceMissing", mReplaceMissing);

   //! What format are the filenames in? Recognizes standard date/time tokens such as %Y for 4-digit
   //! year. Use %H to represent initialization time. In addition, the following are recognized:
   //! %L  location ID
   //! %LC location code
   //! %v  local variable name
   //! %O,%02O,%03O offset (with or without padded 0s infront)
   //! Do not include the file extension
   if(!iOptions.getValue("fileFormat", mFileFormat)) {
      std::stringstream ss;
      ss << "%Y%m%d" << Input::getDefaultFileExtension();
      mFileFormat = ss.str();
   }

   //! Some selectors (such as climatology) must know which dates are available for the input set.
   //! By default this is done by analysing which filenames are available. This option overrides
   //! this automation and instead sets the start date (in YYYYMMDD format).
   //! Must also specify 'endDate'.
   iOptions.getValue("startDate", mStartDate);
   //! See 'startDate'. Set the end date of available dates (in YYYYMMDD format).
   iOptions.getValue("endDate", mEndDate);
   if(Global::isValid(mStartDate) != Global::isValid(mEndDate)) {
      std::stringstream ss;
      ss << "Either both or neither of 'startDate' and 'endDate' must be specified";
      Global::logger->write(ss.str(), Logger::error);
   }

   // Type
   std::string type;
   //! Dataset type. One of 'forecast' or 'observation'.
   iOptions.getRequiredValue("type", type);
   if(type == "observation")
      mType = typeObservation;
   else if(type == "forecast")
      mType = typeForecast;
   else {
      std::stringstream ss;
      ss << "Option 'type=" << type << "' in Input " << getName() << " is not recognized";
      Global::logger->write(ss.str(), Logger::error);
   }
   // Directories
   std::stringstream ss;
   ss << getInputDirectory() << mFolder << "/";
   mDirectory  = ss.str();

   //! Full path of where data files are located
   if(!iOptions.getValue("dataDir", mDataDirectory)) {
      ss << "data";
      mDataDirectory = ss.str();
   }

   //////////////////////////////////
   // Limit which locations to use //
   //////////////////////////////////
   //! Only allow these location IDs
   iOptions.getValues("locations", mAllowLocations);
   //! Remove these location IDs
   iOptions.getValues("removeLocations", mRemoveLocations);
   std::vector<float> latRange(2, Global::MV);
   std::vector<float> lonRange(2, Global::MV);
   //! Only allow locations within this latitude range (min,max)
   iOptions.getValues("latRange", latRange);
   //! Only allow locations within this longitude range (min,max)
   iOptions.getValues("lonRange", lonRange);
   mMinLat = latRange[0];
   mMaxLat = latRange[1];
   mMinLon = lonRange[0];
   mMaxLon = lonRange[1];

   /////////////
   // Caching //
   /////////////
   {
      std::stringstream ss;
      ss << getName() << ":Surrounding";
      mCacheSurroundingLocations.setName(ss.str());
   }
   {
      std::stringstream ss;
      ss << getName() << ":InitTimes";
      mCacheNearestTimeStamp.setName(ss.str());
   }
   {
      std::stringstream ss;
      ss << getName() << ":InitTimesMissing";
      mCacheNearestTimeStampMissing.setName(ss.str());
   }
   {
      std::stringstream ss;
      ss << getName() << ":DateInitMissing";
      mCacheDateInitMissing.setName(ss.str());
   }

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

   if(!hasDataFiles() && !Global::isValid(mStartDate)) {
      std::stringstream ss;
      ss << "Scheme " << getName() << " does not have data files. Therefore 'startDate' and 'endDate' must be specified.";
      Global::logger->write(ss.str(), Logger::error);
   }

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

   // There might not be an init available at iInit. Therefore, use the latest init before iInit.
   int newDate = iDate;
   int newInit = iInit;
   if(mReplaceMissing) {
      bool status = getNearestDateInit(iDate, iInit, newDate, newInit, iCalibrate);
      if(!status)
         // No available date/init
         return Global::MV;
   }

   // Check that we didn't pull a date/init so far back in time that there are no offsets
   float newOffset = iOffset + Global::getTimeDiff(iDate, iInit, 0, newDate, newInit, 0);

   // Since obs from the same valid times (but different dates/offsets), we can try to find
   // another offset from a different day if the desired offset doesn't exist
   std::vector<float> offsets = getOffsets();
   if(getType() == Input::typeObservation && !Global::isValid(getOffsetIndex(newOffset))) {
      for(int i = 0; i < offsets.size(); i++) {
         if(abs(offsets[i] - newOffset) % 24 == 0) {
            newDate = Global::getDate(newDate, newInit, newOffset - offsets[i]);
            newOffset =  offsets[i];
         }
      }
      assert(newInit == 0);
   }
   /*
   if(getType() == Input::typeObservation && newOffset >= 24) {
      newDate = Global::getDate(newDate, newInit, newOffset);
      newOffset =  fmod(newOffset, 24);
   }
   else if(getType() == Input::typeObservation && newOffset < 0) {
      newDate = Global::getDate(newDate, newInit, newOffset);
      newOffset = Global::getOffset(newDate, newOffset);
   }
  */

   float value = Global::MV;
   int variableId;
   bool found = getVariableIdFromVariable(iVariable, variableId);
   assert(found);

   Key::Input key(newDate, newInit, newOffset, locationIndex, iMemberId, variableId);

   // Check if time interpolation is needed
   // For missing offsets, find nearby offsets and interpolate
   // Don't do this for observations because this may not be valid for verification purposes
   if(mAllowTimeInterpolation && getType() != Input::typeObservation && !hasOffset(newOffset)) {
      float lowerOffset = Global::MV;
      float upperOffset = Global::MV;
      int nOffsets = (int) offsets.size();

      if(nOffsets == 0) {
         std::stringstream ss;
         ss << "Input " << getName() << " has no offsets" << std::endl;
         Global::logger->write(ss.str(), Logger::error);
      }

      // Before first offset
      if(newOffset < offsets[0]) {
         value = Global::MV;
      }
      // Later than last offset
      else if(newOffset > offsets[nOffsets-1]) {
         value = Global::MV;
      }
      // In between offsets
      else {
         for(int i = 0; i < nOffsets-1; i++) {
            if(newOffset > offsets[i]) {
               lowerOffset = offsets[i];
               upperOffset = offsets[i+1];
            }
         }

         float lowerValue = getValue(newDate, newInit, lowerOffset, iLocationNum, iMemberId, iVariable, false);
         float upperValue = getValue(newDate, newInit, upperOffset, iLocationNum, iMemberId, iVariable, false);
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
               float f  = (newOffset-lowerOffset) / dx;
               value = lowerValue + dy*f;
            }
         }
         std::stringstream ss;
         ss << "Input.cpp: " << getName() << " does not have offset " << newOffset
            << ": Interpolation between " << lowerOffset << " and " << upperOffset;
         Global::logger->write(ss.str(), Logger::message);
      }
   }
   // Don't interpolate
   else {
      if(!hasOffset(newOffset)) {
         // No data available
         value = Global::MV;
         std::stringstream ss;
         ss << "Input.cpp: " << getName() << " does not have offset " << newOffset;
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
      // Calibrate value
      float scale = getVariableScale(iVariable);
      float offset = getVariableOffset(iVariable);
      value = offset + scale*value;
      assert(!std::isnan(value));
      assert(!std::isinf(value));
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
   std::vector<Member> members = getMembers();
   for(int i = 0; i < (int) members.size(); i++) {
      float value = getValue(iDate, iInit, iOffset, iLocationId, members[i].getId(), iVariable);
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
   iLocations.clear();

   Key::Two<float,float> key(iTarget.getLat(), iTarget.getLon());

   // Load into cache if necessary
   std::vector<std::pair<int,float> > nearbyLocations;
   if(!mCacheSurroundingLocations.isCached(key)) {
      loadSurroundingLocations(iTarget, iNumPoints);
   }
   else {
      // Already loaded, but check if what is loaded has enough locations
      nearbyLocations = mCacheSurroundingLocations.get(key);
      if(nearbyLocations.size() < iNumPoints && getLocations().size() > nearbyLocations.size()) {
         loadSurroundingLocations(iTarget, iNumPoints);
      }
   }
   nearbyLocations = mCacheSurroundingLocations.get(key);

   // Populate array
   const std::vector<Location>& locations = getLocations();
   int N = std::min(iNumPoints, (int) nearbyLocations.size());
   iLocations.resize(N);
   for(int i = 0; i < N; i++) {
      int locationIndex = nearbyLocations[i].first;
      assert(locationIndex < locations.size());
      iLocations[i] = locations[locationIndex];
   }
}

void Input::getSurroundingLocationsByRadius(const Location& iTarget, std::vector<Location>& iLocations, float iRadius) const {
   iLocations.clear();

   Key::Two<float,float> key(iTarget.getLat(), iTarget.getLon());

   // Load into cache if necessary
   if(!mCacheSurroundingLocations.isCached(key)) {
      loadSurroundingLocations(iTarget);
   }
   std::vector<std::pair<int,float> > nearbyLocations = mCacheSurroundingLocations.get(key);
   if(nearbyLocations.back().second < iRadius && getLocations().size() > nearbyLocations.size()) {
      // Load more locations if the furthest nearby location is inside the search radius
      loadSurroundingLocations(iTarget);
   }

   nearbyLocations = mCacheSurroundingLocations.get(key);

   const std::vector<Location>& locations = getLocations();
   for(int i = 0; i < (int) nearbyLocations.size(); i++) {
      int locationIndex = nearbyLocations[i].first;
      float dist = nearbyLocations[i].second;
      if(dist <= iRadius) {
         iLocations.push_back(locations[locationIndex]);
      }
   }
}

void Input::loadSurroundingLocations(const Location& iTarget, int iNum) const {
   Key::Two<float,float> key(iTarget.getLat(), iTarget.getLon());
   const std::vector<Location>& locations = getLocations();

   // Compute distance
   std::vector<std::pair<int, float> > distances; // location index, distance
   for(int i = 0; i < (int) locations.size(); i++) {
      float distance = locations[i].getDistance(iTarget);
      std::pair<int, float> p(i, distance);
      distances.push_back(p);
   }

   // Saving nearest neighbours:
   // All distances have already been computed, therefore store these values in case future calls
   // need more neighbours than 'iNum'. However, this uses too much memory for large datasets. A
   // compromise is to save at least 'storeMinNum' number of neighbours, since later calls are
   // likely to ask for 4.
   int storeMinNum = 4; // Store at least this many neighbours, if possible
   int num = std::max(iNum, storeMinNum);
   if(distances.size() < num) {
      // Sort all distances
      std::sort(distances.begin(), distances.end(), Global::sort_pair_second<int, float>());
   }
   else {
      // Only sort the first 'num' distances (to save time)
      std::partial_sort(distances.begin(), distances.begin()+num,distances.end(), Global::sort_pair_second<int, float>());
      distances.erase(distances.begin() + num, distances.end());
   }
   mCacheSurroundingLocations.add(key, distances);
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
   if(Global::isValid(mStartDate) && Global::isValid(mEndDate)) {
      int date = mStartDate;
      while(date <= mEndDate) {
         iDates.push_back(date);
         date = Global::getDate(date, 24);
      }
   }
   else {
      int startDate   = Global::getDate( Global::getCurrentDate(), 24*20);
      int init      = getInits()[0];
      float offset  = getOffsets()[0];
      int consecutiveMissing = 0;
      if(!hasDataFiles()) {
         std::stringstream ss;
         ss << "Input scheme '" << Input::getSchemeName() << "' has a bug (it doesn't call init). Cannot get dates.";
         Global::logger->write(ss.str(), Logger::critical);
         return false;
      }
      int numFound = 0;
      int date      = startDate;
      int counter   = 0;
      while(counter < 10000 && (consecutiveMissing < 365 || numFound == 0)) {
         Key::Input key(date, init, offset, 0, 0, 0);
         std::string filename = getFilename(key);
         if(boost::filesystem::exists(filename)) {
            iDates.push_back(date);
            numFound++;
            consecutiveMissing = 0;
         }
         else {
            consecutiveMissing++;
         }
         date = Global::getDate(date, -24);
         counter++;
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
   if(iOffset == mLastOffset)
      return mLastOffsetIndex;
   if(mOffsetMap.size() == 0) {
      // Load offset map
      std::vector<float> offsets = getOffsets();
      for(int i = 0 ; i < (int) offsets.size(); i++) {
         mOffsetMap[mOffsets[i]] = i;
      }
   }

   std::map<float, int>::const_iterator it = mOffsetMap.find(iOffset);

   int offsetIndex = Global::MV;
   if(it != mOffsetMap.end()) {
      offsetIndex = it->second;
   }
   mLastOffset = iOffset;
   mLastOffsetIndex = offsetIndex;
   return offsetIndex;
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
//! 1) Read from members.nl
//! 2) Let the scheme define the member
//! 3) Assume a single-member ensemble
std::vector<Member> Input::getMembers() const {
   if(mMembers.size() == 0) {
      readMembersNamelist(mMembers);
      if(mMembers.size() == 0) {
         getMembersCore(mMembers);
         if(mMembers.size() == 0) {
            float resolution = Global::MV;
            std::string model = "";
            int id = 0;
            mMembers.push_back(Member(getName(), resolution, model, id));
         }
         // Check that obs dataset only has one member
         if(getType() == Input::typeObservation) {
            if(mMembers.size() != 1) {
               std::stringstream ss;
               ss << "Input " << getName() << " has " << mMembers.size() << " members. Must be 1";
               Global::logger->write(ss.str(), Logger::error);
            }
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
      std::vector<std::string> tags = nl.getTags();

      mId2Variable.resize(tags.size());
      mId2LocalVariable.resize(tags.size());
      for(int i = 0; i < (int) tags.size(); i++) {
         std::string tag = tags[i];
         if(tag != "") {
            Options opt;
            nl.getOptions(tag, opt);
            std::string value;
            opt.getValue("name", value);
            mVariable2LocalVariable[tag] = value;
            mVariable2Id[tag] = i;
            mId2Variable[i] = tag;
            mLocalVariable2Id[value] = i;
            mId2LocalVariable[i] = value;

            float scale  = 1;
            float offset = 0;
            opt.getValue("scale", scale);
            opt.getValue("offset", offset);

            mVariableScale[tag] = scale;
            mVariableOffset[tag] = offset;
            assert(Global::isValid(scale));
            assert(Global::isValid(offset));

            mVariables.push_back(tag);
         }
      }
      assert(mVariables.size() > 0);
   }
   return mVariables;
}
//! 1) Read from locations.nl
//! 2) Let scheme define locations
const std::vector<Location>& Input::getLocations() const {
   if(mLocations.size() == 0) {
      readLocationsNamelist(mLocations);

      if(mLocations.size() == 0) {
         getLocationsCore(mLocations);
      }
      if(mAllowLocations.size() > 0) {
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
      if(mRemoveLocations.size() > 0) {
         for(int i = 0; i < (int) mRemoveLocations.size(); i++) {
            for(int j = (int) mLocations.size() - 1; j >= 0; j--) {
               if(mRemoveLocations[i] == mLocations[j].getId()) {
                  mLocations.erase(mLocations.begin() + j);
               }
            }
         }
      }
      // Remove stations with missing information
      for(int i = mLocations.size()-1; i >= 0; i--) {
         Location loc = mLocations[i];
         if(!Global::isValid(loc.getLat()) ||
               !Global::isValid(loc.getLon())) {
            mLocations.erase(mLocations.begin() + i);
            std::stringstream ss;
            ss << "Location " << loc.getId() << " in dataset '" << getName() << "' is missing lat/lon information. Location ignored.";
            Global::logger->write(ss.str(), Logger::warning);
         }
      }
      // Remove stations outside lat/lon ranges
      for(int i = mLocations.size()-1; i >= 0; i--) {
         Location loc = mLocations[i];
         if((Global::isValid(mMinLat) && loc.getLat() < mMinLat) ||
            (Global::isValid(mMaxLat) && loc.getLat() > mMaxLat) ||  
            (Global::isValid(mMinLon) && loc.getLon() < mMinLon) ||  
            (Global::isValid(mMaxLon) && loc.getLon() > mMaxLon)) {
            mLocations.erase(mLocations.begin() + i);
         }
      }
   }
   if(mLocations.size() == 0) {
      std::stringstream ss;
      ss << "Input " << getName() << "' has no locations";
      Global::logger->write(ss.str(), Logger::warning);
   }
   return mLocations;
}

//! 1) Get offsets from 'offsets=' attribute in runs.nl
//! 2) Let scheme define offsets
std::vector<float> Input::getOffsets() const {
   if(mOffsets.size() == 0) {
      getOffsetsCore(mOffsets);
      assert(mOffsets.size() > 0);
   }
   return mOffsets;
}

std::vector<int> Input::getInits() const {
   if(mInits.size() == 0) {
      readInitsNamelist(mInits);
      if(mInits.size() == 0) {
         getInitsCore(mInits);
      }
      assert(mInits.size() > 0);
   }
   return mInits;
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


void Input::readLocationsNamelist(std::vector<Location>& iLocations) const {
   std::string filename = getNamelistFilename("locations");
   Namelist nl(filename);
   std::vector<std::string> tags = nl.getTags();

   for(int i = 0; i < tags.size(); i++) {
      std::string id = tags[i];

      Options opt;
      nl.getOptions(id, opt);
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

      int idInt = Global::getInt(id);

      Location loc(getName(), idInt, lat, lon);
      loc.setElev(elev);
      loc.setName(name);
      if(code.size()) {
         loc.setCode(code);
      }
      iLocations.push_back(loc);
   }
}

void Input::readInitsNamelist(std::vector<int>& iInits) const {
   std::string filename = getNamelistFilename("inits");
   if((!boost::filesystem::exists(filename))) {
      // If namelist doesn't exist, assume init is 0
      iInits.push_back(0);
   }
   else {
      Namelist nl(filename);
      std::vector<std::string> tags = nl.getTags();
      for(int i = 0; i < tags.size(); i++) {
         int init = Global::getInt(tags[i]);
         iInits.push_back(init);
      }
   }
   // Observations should not have any inits other than 0
   if(mType == Input::typeObservation && (iInits.size() != 1 || iInits[0] != 0)) {
      std::stringstream ss;
      ss << "Input dataset '" << mName << "' can only have an init of 0 as it contains observations";
      Global::logger->write(ss.str(), Logger::error);
   }
}

void Input::readMembersNamelist(std::vector<Member>& iMembers) const {
   // Members
   std::string filename = getNamelistFilename("members");
   Namelist nlMembers(filename);
   std::vector<std::string> tags = nlMembers.getTags();
   for(int i = 0; i < tags.size(); i++) {
      std::string tag = tags[i];
      Options options;
      nlMembers.getOptions(tag, options);
      std::string model = "";
      options.getValue("model", model);
      float res = Global::MV;
      options.getValue("resolution", res);
      int id = Global::getInt(tag);
      // TODO: Handle this better
      // members.nl uses an id as the tag. However, if these are not consecutive the caching
      // crashes, because it uses the member Id as an index into an array.
      if(id != i) {
         std::stringstream ss;
         ss << "Member ids in " << filename << " must be consequtive starting at 0";
         Global::logger->write(ss.str(), Logger::error);
      }
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

void Input::write(const Data& iData, const Input& iDimensions, const std::vector<Location>& iLocations, int iDate, int iInit) const {
   writeCore(iData, iDimensions, iLocations, iDate, iInit);
}
void Input::write(const Data& iData, const std::vector<Location>& iLocations, int iDate, int iInit) const {
   writeCore(iData, *this, iLocations, iDate, iInit);
}

void Input::writeCore(const Data& iData, const Input& iDimensions, const std::vector<Location>& iLocations, int iDate, int iInit) const {
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

std::string Input::getLocationFilename() const {
   if(mLocationFilename == "")
      return getSampleFilename();
   else {
      std::stringstream ss;
      if(mLocationFilename[0] != '/') {
         ss << getDirectory();
      }
      ss << mLocationFilename;
      return ss.str();
   }
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
      const std::vector<Location>& locations = getLocations();
      for(int i = 0 ; i < (int) locations.size(); i++) {
         mLocationMap[locations[i].getId()] = i;
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

std::string Input::getFilename(const Key::Input& iKey) const {
   std::stringstream ss;
   ss << getDataDirectory() << "/";

   std::string fileFormat = mFileFormat;

   // Fill in variable name
   std::string variable;
   Input::getLocalVariableName(iKey.variable, variable);
   boost::replace_all(fileFormat, "%v", variable);

   // Fill in location code
   const std::vector<Location>& locations = getLocations();
   assert(Global::isValid(iKey.location));
   assert(iKey.location < locations.size() && iKey.location >= 0);
   std::string locationCode = locations[iKey.location].getCode();
   boost::replace_all(fileFormat, "%LC", locationCode);

   // Fill in location name
   std::stringstream ss2;
   ss2 << locations[iKey.location].getId();
   std::string locationNum = ss2.str();
   boost::replace_all(fileFormat, "%L", locationNum);

   struct tm timeInfo;
   timeInfo.tm_year = Global::getYear(iKey.date)  - 1900;
   timeInfo.tm_mon  = Global::getMonth(iKey.date) - 1;
   timeInfo.tm_mday = Global::getDay(iKey.date);
   timeInfo.tm_hour = iKey.init;
   timeInfo.tm_min  = 0;
   timeInfo.tm_sec  = 0;
   char buffer[101];
   strftime(buffer, 100, fileFormat.c_str(), &timeInfo);
   ss << buffer << getFileExtension();

   std::string filename = ss.str();

   // Fill in offset
   {
      std::stringstream ss0;
      ss0 << (int) iKey.offset;
      boost::replace_all(filename, "%O", ss0.str());
   }
   {
      std::stringstream ss0;
      ss0 << std::setfill('0') << std::setw(2) << (int) iKey.offset;
      boost::replace_all(filename, "%02O", ss0.str());
   }
   {
      std::stringstream ss0;
      ss0 << std::setfill('0') << std::setw(3) << (int) iKey.offset;
      boost::replace_all(filename, "%03O", ss0.str());
   }

   // std::cout << filename << std::endl;
   return filename;
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
int Input::getNumInits() const {
   if(!Global::isValid(mNumInits))
      mNumInits = getInits().size();
   return(mNumInits);
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

bool Input::getNearestDateInit(int iDate, int iInit, int& iNewDate, int& iNewInit, bool iHandleDelay) const {
   Key::Three<int,int,bool> key0(iDate, iInit, iHandleDelay);

   // Don't search for other inits for observations
   if(getType() == typeObservation) {
      iNewDate = Global::getDate(iDate, iInit);
      iNewInit = 0;

      // Cache result
      Key::DateInit key1(iNewDate, iNewInit);
      mCacheNearestTimeStamp.add(key0,key1);
      return true;
   }

   float delay = mInitDelay;
   if(!iHandleDelay)
      delay = 0;

   if(mCacheNearestTimeStampMissing.isCached(key0)) {
      // We have already determined that there are no suitable time stamps
      return false;
   }
   // Retrive from cache
   if(mCacheNearestTimeStamp.isCached(key0)) {
      Key::DateInit key1 = mCacheNearestTimeStamp.get(key0);
      iNewDate = key1.mDate;
      iNewInit = key1.mLocationId;
      return true;
   }

   std::vector<int> inits = getInits();
   std::vector<float> offsets = getOffsets();
   float maxOffset = offsets[offsets.size()-1];
   float minOffset = offsets[0];

   int date = iDate;
   int init = Global::MV;
   int dateDiff = 0;
   assert(iInit >= 0 && iInit < 24);

   // Start at the most recent point in time and go back in time to find a suitable date/init
   int counter = 0;
   bool found = false;
   bool usingMostRecent = true;
   int  mostRecentDate = Global::MV;
   int  mostRecentInit = Global::MV;
   //std::cout << "Finding available data/init for " << getName() << " (" << iDate << ", " << iInit << ", " << iOffset << ")" << std::endl;
   while(true) {
      for(int i = inits.size()-1; i >= 0; i--) {
         init = inits[i];
         float diff = Global::getTimeDiff(date, init + delay,0, iDate, iInit, 0);
         if(diff <= 0) {
            // Current date/init is available

            // Check that we haven't gone too far back in time
            if(Global::getTimeDiff(iDate, iInit, minOffset, date, init, maxOffset) > 0) {
               mCacheNearestTimeStampMissing.add(key0,1);
               std::stringstream ss;
               ss << "No suitable date/init for " << getName() << " for " << iDate << ":" << iInit;
               Global::logger->write(ss.str(), Logger::warning);
               return false;
            }

            // Check that date/init is available
            if(!isMissing(date, init)) {
               found = true;
               break; // Exit for loop
            }
            else if(!mReplaceMissing) {
               // We are not allowed to use datasets older than the most recent
               mCacheNearestTimeStampMissing.add(key0,1);
               return false;
            }
            else if(usingMostRecent) {
               // We have at least one missing date/init, we are therefore definitely not
               // using the most recent date/init
               usingMostRecent = false;
               mostRecentInit = init;
               mostRecentDate = date;
            }
         }
         counter++;
         if(counter > 1000) {
            mCacheNearestTimeStampMissing.add(key0,1);
            return false;
         }
      }
      if(found)
         break;
      date = Global::getDate(date, 0, -24); 
      dateDiff++;
   }
   assert(found);

   iNewDate = date;
   iNewInit = init;

   if(!usingMostRecent) {
      std::stringstream ss;
      ss << "The most recent date/init (" << mostRecentDate << ":" << mostRecentInit << ") is not available for " << getName() << ". Using "
         << iNewDate << ":" << iNewInit << " instead.";
      Global::logger->write(ss.str(), Logger::warning);
   }

   // Cache result
   Key::DateInit key1(iNewDate, iNewInit);
   mCacheNearestTimeStamp.add(key0,key1);

   assert(Global::isValid(iNewDate));
   assert(Global::isValid(iNewInit));
   //std::cout << mName << " (" << iDate << ", " << iInit << ", " << iOffset << ") -> "
   //          << "(" << iNewDate << ", " << iNewInit << ", " << iNewOffset << ")" << std::endl;
   return true;
}

bool Input::isMissing(int iDate, int iInit) const {
   Key::DateInit key(iDate, iInit);
   if(mCacheDateInitMissing.isCached(key))
      return mCacheDateInitMissing.get(key);

   float value = getValueCore(Key::Input(iDate, iInit, getOffsets()[0], 0, 0, 0));
   bool isMissing = !Global::isValid(value);
   mCacheDateInitMissing.add(key, isMissing);
   return isMissing;
}
