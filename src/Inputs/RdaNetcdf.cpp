#include "RdaNetcdf.h"
#include "../DataKey.h"
#include "../Location.h"
#include "../Member.h"
#include "../Options.h"
#include <boost/date_time/gregorian/gregorian_types.hpp>
#include <boost/date_time/date_duration.hpp>

// TODO: Error checking on Netcdf commands
InputRdaNetcdf::InputRdaNetcdf(const Options& iOptions, const Data& iData) : Input(iOptions, iData),
      mTimeTolerance(900) {
   mFileExtension ="nc";

   // Caching of other variables not implemented
   if(mCacheOtherVariables) {
      Global::logger->write("InputRdaNetcdf: Cannot cache other variables", Logger::warning);
      mCacheOtherVariables = false;
   }
   
   init();
}

void InputRdaNetcdf::loadLocations() const {
   std::string filename = getSampleFilename();
   NcFile ncfile(filename.c_str());
   assert(ncfile.is_valid());
   NcVar* ncLats = ncfile.get_var("latitude");
   NcVar* ncLons = ncfile.get_var("longitude");
   NcVar* ncElevs = ncfile.get_var("altitude");
   NcVar* ncNames = ncfile.get_var("station_id");

   NcDim* ncNamesDim = ncfile.get_dim("id_len");
   int namesLength = ncNamesDim->size();

   NcDim* ncLocationDim = ncfile.get_dim("station");
   int numLocations = ncLocationDim->size();

   float* lats  = new float[numLocations];
   float* lons  = new float[numLocations];
   float* elevs = new float[numLocations];
   char* names  = new char[numLocations*namesLength];

   long count[2] = {numLocations, namesLength};
   ncLats->get(lats, count);
   ncLons->get(lons, count);
   ncElevs->get(elevs, count);
   ncNames->get(names, count);

   mLocations.resize(numLocations);
   for(int i = 0; i < numLocations; i++) {
      int   id   = i;
      float lat  = lats[i];
      float lon  = lons[i];
      float elev = elevs[i];
      Location loc(mName, id, lat, lon, elev);
      mLocations[i] = loc;
      int nameIndex = i*namesLength;
      std::string name = std::string(&names[nameIndex], namesLength);
      mLocationNames[name] = i;
   }
   delete[] lats;
   delete[] lons;
   delete[] elevs;
   delete[] names;
   ncfile.close();
}

void InputRdaNetcdf::loadOffsets() const {
   for(int i = 0; i < 24; i++) {
      mOffsets.push_back(i);
   }
}

void InputRdaNetcdf::loadMembers() const {
   mMembers.clear();
   Member member(mName, Global::MV, "", 0);
   mMembers.push_back(member);
}

float InputRdaNetcdf::getValueCore(const Key::Input& iKey) const {
   float returnValue = Global::MV;

   std::string filename = getFilename(iKey);
   NcFile ncfile(filename.c_str());

   std::string varName = mId2LocalVariable[iKey.variable];
   Key::Input key = iKey;

   // Pre-fill incase file does not contain some keys
   for(int o = 0; o < mOffsets.size(); o++) {
      key.offset = mOffsets[o];
      for(key.location = 0; key.location < mLocations.size(); key.location++) {
         if((mCacheOtherOffsets || iKey.offset == key.offset) ||
               (mCacheOtherLocations || iKey.location == key.location)) {
            Input::addToCache(key, Global::MV);
         }
      }
   }

   if(ncfile.is_valid() && varName != "") {
      // Record data
      NcVar* ncvar          = ncfile.get_var(varName.c_str());
      NcVar* ncTimes        = ncfile.get_var("time_observation");
      NcVar* ncStationIds   = ncfile.get_var("parent_index");
      NcDim* ncNamesDim     = ncfile.get_dim("id_len");
      NcDim* ncRecordsDim   = ncfile.get_dim("recNum");
      long   numRecords = ncRecordsDim->size();
      float  values[numRecords];
      float  stationIds[numRecords];
      int    times[numRecords];

      long count[1] = {numRecords};
      ncvar->get(values, count);
      ncTimes->get(times, count);
      ncStationIds->get(stationIds, count);

      // Station data
      NcVar* ncNames        = ncfile.get_var("station_id");
      NcDim* ncLocationDim  = ncfile.get_dim("station");
      long   numCurrLocations = ncLocationDim->size();
      long   namesLength      = ncNamesDim->size();
      char   names[numCurrLocations*namesLength];

      long count2[2] = {numCurrLocations, namesLength};
      ncNames->get(names, count2);

      ncfile.close();
      // Set all values to missing
      std::vector<float> vec;
      vec.resize(mLocations.size()*mOffsets.size(), Global::MV);

      // Read data
      for(int i = 0; i < numRecords; i++) {
         int id = stationIds[i];
         int namesIndex = id*namesLength;
         std::string name = std::string(&names[namesIndex], namesLength);
         std::map<std::string,int>::const_iterator it = mLocationNames.find(name);
         if(it != mLocationNames.end()) {
            key.location = it->second;
            key.member = 0;
            int time = times[i];

            int year  = Global::getYear(key.date);
            int month = Global::getMonth(key.date);
            int day   = Global::getDay(key.date);
            boost::gregorian::date epochDate(1970, 1, 1);
            boost::gregorian::date currDate(year, month, day);
            boost::gregorian::date_period diff(epochDate, currDate);
            int daysSinceEpoch = diff.length().days();

            int offsetIndex = round((float) (time - 86400*daysSinceEpoch)/3600);
            int secondsOffHour = (time - 86400*daysSinceEpoch) % 3600;
            if(secondsOffHour < 0) 
               secondsOffHour = 3600 + secondsOffHour;
            if(secondsOffHour > 1800) {
               secondsOffHour = 3600 - secondsOffHour;
            }
            assert(offsetIndex >= 0);
            if(secondsOffHour < mTimeTolerance && offsetIndex < 24) {
               assert(key.location < mLocations.size());
               int ind = offsetIndex*mLocations.size() + key.location;
               assert(offsetIndex >= 0 && offsetIndex < mOffsets.size());
               key.offset = mOffsets[offsetIndex];
               assert(ind < (int) vec.size() && ind >= 0);
               //std::cout << key.location << " " << key.offset << std::endl;
               // Rda dataset uses a different missing value indicator
               if(values[i] == mMV)
                  values[i] = Global::MV;
               if((mCacheOtherOffsets || iKey.offset == key.offset) ||
                  (mCacheOtherLocations || iKey.location == key.location)) {
                  Input::addToCache(key, values[i]);
                  if(iKey == key) {
                     returnValue = values[i];
                  }
               }
            }
         }
      }
   }
   return returnValue;
}

void InputRdaNetcdf::optimizeCacheOptions() {
   mCacheOtherOffsets   = true;
   mCacheOtherLocations = true;
   mCacheOtherMembers   = true;
   mCacheOtherVariables = false;
}

std::string InputRdaNetcdf::getFilename(const Key::Input& rKey) const {
   std::stringstream ss(std::stringstream::out);
   ss << mDataDirectory << rKey.date << ".nc";
   return ss.str();
}
