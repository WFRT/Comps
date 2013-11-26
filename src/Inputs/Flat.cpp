#include "Flat.h"
#include "../Member.h"
#include "../Location.h"
#include "../Options.h"

InputFlat::InputFlat(const Options& iOptions, const Data& iData) :
      Input(iOptions, iData),
      mUseCodeInFilename(false) {
   //! Is the station code used in the filename instead of the stationID?
   iOptions.getValue("useCodeInFilename", mUseCodeInFilename);
   if(iOptions.hasValue("cacheOtherMembers") || iOptions.hasValue("cacheOtherOffsets") ||
      iOptions.hasValue("cacheOtherLocations") || iOptions.hasValue("cacheOtherVariables")) {
      std::stringstream ss;
      ss << "Caching options for dataset '" << getName()
         << "' are ignored as InputFlat decides its own caching";
      Global::logger->write(ss.str(), Logger::warning);

   }
   init();
}

float InputFlat::getValueCore(const Key::Input& iKey) const {
   float returnValue = Global::MV;

   std::string filename = getFilename(iKey);
   std::vector<float> values;

   std::vector<float> offsets = getOffsets();
   std::vector<Member> members = getMembers();

   std::ifstream ifs(filename.c_str(), std::ifstream::in);
   if(!ifs.good()) {

      // Missing file
      std::vector<float> values;
      Key::Input key = iKey;
      int offsetId = getOffsetIndex(iKey.offset);
      for(int i  = mCacheOtherOffsets ? 0 : offsetId;
              i <= (mCacheOtherOffsets ? offsets.size()-1 : offsetId) ;
              i++) {
         key.offset = offsets[i];
         for(key.member = mCacheOtherMembers ? 0 : iKey.member;
             key.member <= (mCacheOtherMembers ? members.size() - 1 : iKey.member);
             key.member++) {
            Input::addToCache(key, Global::MV);
         }
      }
      std::stringstream ss;
      ss << "InputFlat: File " << filename << " missing";
      Global::logger->write(ss.str(), Logger::message);
   }
   else {
      // Loop over each line
      Key::Input key = iKey;
      int offsetId = 0;
      while(ifs.good()) {
         char line[10000];
         ifs.getline(line, 10000, '\n');
         if(ifs.good() && line[0] != '#') {
            if(offsetId >= offsets.size()) {
               std::stringstream ss;
               ss << "InputFlat: File '" << filename << "' has too many lines" << std::endl;
               Global::logger->write(ss.str(), Logger::warning);
               break;
            }
            assert(offsetId < offsets.size());
            key.offset = offsets[offsetId];
            std::stringstream ss(line);
            // Loop over each value
            key.member = 0;
            while(ss.good()) {
               float value;
               if(ss >> value) {
                  Input::addToCache(key, value);
               }
               if(key.offset == iKey.offset && key.member == iKey.member) {
                  returnValue = value;
               }
               key.member++;
            }
         }
         offsetId++;
      }
      offsetId--;
      if(offsetId < offsets.size()-1) {
         std::stringstream ss;
         ss << "InputFlat: File '" << filename << "' has too few rows. Assume last rows are missing values";
         Global::logger->write(ss.str(), Logger::warning);
         while(offsetId < offsets.size()) {
            key.offset = offsets[offsetId];
            for(key.member = 0; key.member < getNumMembers(); key.member++) {
               Input::addToCache(key, Global::MV);
            }
            offsetId++;
         }
      }
   }
   return returnValue;
}

std::string InputFlat::getFilename(const Key::Input& iKey) const {
   std::stringstream ss(std::stringstream::out);
   std::string localVariable;
   bool found = getLocalVariableName(iKey.variable, localVariable);
   assert(found);

   std::vector<Location> locations = getLocations();

   assert(iKey.location < locations.size());
   ss << getDataDirectory(iKey);
   ss << iKey.date << "_";
   if(mUseCodeInFilename) {
      std::string locationCode = locations[iKey.location].getCode();
      ss << locationCode;
   }
   else {
      int locationNum = locations[iKey.location].getId();
      ss << locationNum;
   }
   ss << "_" << localVariable << Input::getFileExtension();
   return ss.str();
}
