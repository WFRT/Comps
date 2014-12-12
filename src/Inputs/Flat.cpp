#include "Flat.h"
#include "../Member.h"
#include "../Location.h"
#include "../Options.h"
#include "../Data.h"

InputFlat::InputFlat(const Options& iOptions) :
      Input(iOptions) {

   //! What offsets do each row in the text file represent?
   std::vector<float> offsets;
   iOptions.getRequiredValues("offsets", offsets);

   // Set caching settings
   if(iOptions.hasValue("cacheOtherMembers") || iOptions.hasValue("cacheOtherOffsets") ||
      iOptions.hasValue("cacheOtherLocations") || iOptions.hasValue("cacheOtherVariables")) {
      std::stringstream ss;
      ss << "Caching options for dataset '" << getName()
         << "' are ignored as InputFlat decides its own caching";
      Global::logger->write(ss.str(), Logger::warning);

   }
   mCacheOtherMembers   = true;
   mCacheOtherOffsets   = true;
   mCacheOtherVariables = false;
   mCacheOtherLocations = false;

   iOptions.check();
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
      for(int i = 0; i < offsets.size(); i++) {
         key.offset = offsets[i];
         for(key.member = 0; key.member < members.size(); key.member++) {
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
      // Deal with truncated files
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

void InputFlat::writeCore(const Data& iData, int iDate, int iInit, const std::vector<float>& iOffsets, const std::vector<Location>& iLocations, const std::vector<Member>& iMembers, const std::vector<std::string>& iVariables) const {
   // Write each variable
   for(int v = 0; v < (int) iVariables.size(); v++) {
      std::string variable = iVariables[v];
      std::string localVariable;
      bool found = getLocalVariableName(variable, localVariable);
      if(!found) {
         std::stringstream ss;
         ss << "InputNetcdf::write: Do not know what to map " << variable << " to. Skipping.";
         Global::logger->write(ss.str(), Logger::message);
      }
      else {
         int variableId = Global::MV;
         bool found = getVariableIdFromVariable(variable, variableId);
         // Populate values;
         for(int l = 0; l < (int) iLocations.size(); l++) {
            // Set up file
            Key::Input key;
            key.date = iDate;
            key.init = iInit;
            key.location = l;
            key.variable = variableId;
            std::string filename = getFilename(key);
            Global::createDirectory(Global::getDirectory(filename));
            std::ofstream ofs(filename.c_str(), std::ios_base::out);

            for(int o = 0; o < (int) iOffsets.size(); o++) {
               float offset = iOffsets[o];
               for(int m = 0; m < (int) iMembers.size(); m++) {
                  float value = iData.getValue(iDate, iInit, offset, iLocations[l], iMembers[m], variable);
                  ofs << value << " ";
               }
               ofs << std::endl;
            }
         }
      }
   }
}
