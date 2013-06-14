#include "Flat.h"
#include "../DataKey.h"
#include "../Member.h"
#include "../Location.h"
#include "../Options.h"

InputFlat::InputFlat(const Options& rOptions, const Data& iData) : Input(rOptions, iData) ,
      mUseCodeInFilename(false) {
   mCache.setName(mName);
   mCache.setMaxSize(mMaxCacheSize);
   rOptions.getValue("useCodeInFilename", mUseCodeInFilename);
   init();
}
InputFlat::~InputFlat() {}

float InputFlat::getValueCore(const Key::Input& iKey) const {
   float returnValue = Global::MV;

   std::string filename = getFilename(iKey);
   std::vector<float> values;

   std::ifstream ifs(filename.c_str(), std::ifstream::in);
   if(!ifs.good()) {
      // Missing file
      std::vector<float> values;
      Key::Input key = iKey;
      int offsetId = getOffsetIndex(iKey.offset);
      for(int i  = mCacheOtherOffsets ? 0 : offsetId;
              i <= (mCacheOtherOffsets ? mOffsets.size()-1 : offsetId) ;
              i++) {
         key.offset = mOffsets[iKey.offset];
         for(key.member = mCacheOtherMembers ? 0 : iKey.member;
             key.member <= (mCacheOtherMembers ? mMembers.size() - 1 : iKey.member);
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
         key.offset = mOffsets[offsetId];
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
   std::string localVariableName = mId2LocalVariable[iKey.variable];
   assert(iKey.location < mLocations.size());
   if(mUseCodeInFilename) {
      std::string locationCode = mLocations[iKey.location].getCode();
      ss << mDataDirectory << iKey.date << "_" << locationCode << "_" << localVariableName << Input::getFileExtension();
   }
   else {
      int locationNum = mLocations[iKey.location].getId();
      ss << mDataDirectory << iKey.date << "_" << locationNum << "_" << localVariableName << Input::getFileExtension();
   }
   return ss.str();
}
