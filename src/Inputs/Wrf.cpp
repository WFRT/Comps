#include "Wrf.h"
#include "../Location.h"
#include "../Member.h"
#include "../Options.h"

InputWrf::InputWrf(const Options& rOptions, const Data& iData) : Input(rOptions, iData) {
   mFileExtension ="nc";

   mCache.setName(mName);
   mCache.setMaxSize(mMaxCacheSize);
   init();
}

void InputWrf::loadLocations() const {
   mLocations.clear();
   std::string filename = getConfigFilename("sample");
   NcFile ncfile(filename.c_str());
   assert(ncfile.is_valid());
   NcVar* ncLats = ncfile.get_var("XLAT");
   NcVar* ncLons = ncfile.get_var("XLONG");
   //NcVar* ncElevs = ncfile.get_var("HGT");

   NcDim* ncOffsetDim = ncfile.get_dim("Time");
   NcDim* ncXDim = ncfile.get_dim("west_east");
   NcDim* ncYDim = ncfile.get_dim("south_north");
   mNumLocX      = ncXDim->size();
   mNumLocY      = ncYDim->size();
   int numLoc    = mNumLocX*mNumLocY;

   float* lats  = new float[numLoc];
   float* lons  = new float[numLoc];
   long count[3] = {1,mNumLocY, mNumLocX};
   ncLats->set_cur(0, 0, 0);
   ncLats->get(lats, count);
   ncLons->set_cur(0, 0, 0);
   ncLons->get(lons, count);
   //ncElevs->get(elevs, count);

   for(int i = 0; i < numLoc; i++) {
      int   id   = i;
      float lat  = lats[i];
      float lon  = lons[i];
      float elev = 0;
      Location loc(mName, id, lat, lon, elev);
      mLocations.push_back(loc);
   }
   delete[] lats;
   delete[] lons;
   ncfile.close();
}

void InputWrf::loadOffsets() const {

   // Get offset information from the "Times" variable. Assume that:
   // -- these are ordered
   // -- the first time is 0
   std::string filename = getConfigFilename("sample");

   NcFile ncfile(filename.c_str());
   NcDim* dimDateStrLen = ncfile.get_dim("DateStrLen");
   NcDim* dimTime = ncfile.get_dim("Time");
   NcVar* varTimes = ncfile.get_var("Times");
   int NTime = dimTime->size();
   int NStr  = dimDateStrLen->size();
   long count[2] = {NTime, NStr};
   char* timesArray = new char[NTime*NStr + 1];
   varTimes->get(timesArray, count);
   std::string times(timesArray);
   delete timesArray;

   // Date/time of first offset (used to compute time differences)
   int   d0 = Global::MV;
   float o0 = 0;

   // Loop over all output times in file
   for(int i = 0; i < NTime; i++) {
      int base = i*NStr;
      int year   = getSubstring(times, base, 4);
      int month  = getSubstring(times, base+5,2);
      int day    = getSubstring(times, base+8,2);
      int hour   = getSubstring(times, base+11,2);
      int minute = getSubstring(times, base+14,2);
      int second = getSubstring(times, base+17,2);

      // First output offset
      if(i == 0) {
         d0 = year*10000 + month*100 + day;
         o0 = hour + (float) minute/60 + (float) second/3600;
      }

      int d1   = year*10000 + month*100 + day;
      float o1 = hour + (float) minute/60 + (float) second/3600;

      // Difference from first output time
      float offset = Global::getTimeDiff(d1, 0, o1, d0, 0, o0);
      assert(offset >= 0);
      mOffsets.push_back(offset);
   }
}

void InputWrf::loadMembers() const {
   std::string filename = getConfigFilename("sample");
   NcFile ncfile(filename.c_str());
   assert(ncfile.is_valid());

   float res = ncfile.get_att("DX")->as_float(0)/1000;

   mMembers.clear();
   Member member(mName, res, "");
   mMembers.push_back(member);

   ncfile.close();
}

float InputWrf::getValueCore(const Key::Input& iKey) const {
   assert(iKey.member == 0);
   float returnValue = Global::MV;

   std::string filename = getFilename(iKey);
   NcFile ncfile(filename.c_str());


   std::string varName = mId2LocalVariable[iKey.variable];
   Key::Input key = iKey;

   // Pre-fill incase file does not contain some keys (i.e. all offsets)
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
      float values[mOffsets.size()*mLocations.size()];
      NcVar* ncvar = ncfile.get_var(varName.c_str());
      NcDim* ncXDim = ncfile.get_dim("west_east");
      NcDim* ncYDim = ncfile.get_dim("south_north");
      int numLocX   = ncXDim->size();
      int numLocY   = ncYDim->size();

      // TODO: Nice message
      assert(mNumLocX == numLocX);
      assert(mNumLocY == numLocY);

      long count[3] = {mOffsets.size(), mNumLocY, mNumLocX};
      // NETCDF: last dimension varies fastest
      ncvar->get(values, count);
      ncfile.close();

      int index = 0;
      for(int o = 0; o < mOffsets.size(); o++) {
         key.offset = mOffsets[o];
         for(key.location = 0; key.location < mLocations.size(); key.location++) {
            if((mCacheOtherOffsets || iKey.offset == key.offset) ||
                  (mCacheOtherLocations || iKey.location == key.location)) {
               Input::addToCache(key, values[index]);
               if(iKey == key) {
                  returnValue = values[index];
               }
            }
            index++;
         }
      }
   }
   return returnValue;
}

std::string InputWrf::getFilename(const Key::Input& iKey) const {
   std::stringstream ss(std::stringstream::out);
   ss << mDataDirectory << iKey.date << ".nc";
   return ss.str();
}

void InputWrf::optimizeCacheOptions() {
   mCacheOtherOffsets   = true;
   mCacheOtherLocations = true;
   mCacheOtherMembers   = true;
   mCacheOtherVariables = false;
}

int InputWrf::getSubstring(const std::string iString, int iStart, int iLength) const {
   int value;
   std::stringstream ss(iString.substr(iStart,iLength));
   ss >> value;
   return value;
}
