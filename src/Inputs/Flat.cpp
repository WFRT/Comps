#include "Flat.h"
#include "../DataKey.h"
#include "../Member.h"
#include "../Location.h"
#include "../Options.h"

InputFlat::InputFlat(const Options& iOptions, const Data& iData) :
      Input(iOptions, iData),
      mUseCodeInFilename(false) {
   //! Is the station code used in the filename instead of the stationID?
   iOptions.getValue("useCodeInFilename", mUseCodeInFilename);
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
         key.offset = offsets[offsetId];
         char line[10000];
         ifs.getline(line, 10000, '\n');
         if(ifs.good() && line[0] != '#') {
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
   if(mUseCodeInFilename) {
      std::string locationCode = locations[iKey.location].getCode();
      ss << getDataDirectory() << iKey.date << "_" << locationCode << "_" << localVariable << Input::getFileExtension();
   }
   else {
      int locationNum = locations[iKey.location].getId();
      ss << getDataDirectory() << iKey.date << "_" << locationNum << "_" << localVariable << Input::getFileExtension();
   }
   return ss.str();
}
