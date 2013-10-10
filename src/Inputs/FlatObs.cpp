#include "FlatObs.h"
#include "../Member.h"
#include "../Location.h"
#include "../Options.h"

InputFlatObs::InputFlatObs(const Options& iOptions, const Data& iData) :
      Input(iOptions, iData),
      mUseCodeInFilename(false) {
   //! Is the station code used in the filename instead of the stationID?
   iOptions.getValue("useCodeInFilename", mUseCodeInFilename);
   init();
}

float InputFlatObs::getValueCore(const Key::Input& iKey) const {
   assert(iKey.member == 0);
   float returnValue = Global::MV;

   std::string filename = getFilename(iKey);
   std::cout << filename << std::endl;
   std::ifstream ifs(filename.c_str(), std::ifstream::in);

   std::vector<float> offsets = getOffsets();

   // Initialize with empty values
   std::vector<int> dates;
   Input::getDates(dates);
   Key::Input key = iKey;
   for(int i = 0; i < (int) dates.size(); i++) {
      key.date = dates[i];
      for(int k = 0; k < (int) offsets.size(); k++) {
         key.offset = offsets[k];
         Input::addToCache(key, Global::MV);
      }
   }

   // Populate if file exists
   if(ifs.good()) {
      // Loop over each line
      Key::Input key = iKey;
      while(ifs.good()) {
         char line[10000];
         ifs.getline(line, 10000, '\n');
         if(ifs.good() && line[0] != '#') {
            std::stringstream ss(line);
            // Loop over each value
            while(ss.good()) {
               float value;
               ss >> key.date >> key.offset >> value;
               Input::addToCache(key, value);
               if(key.offset == iKey.offset && key.date == iKey.date) {
                  returnValue = value;
               }
            }
         }
      }
   }
   ifs.close();
   return returnValue;
}

std::string InputFlatObs::getFilename(const Key::Input& iKey) const {
   std::string localVariable;
   bool found = getLocalVariableName(iKey.variable, localVariable);
   assert(found);

   std::vector<Location> locations = getLocations();
   assert(iKey.location < locations.size());
   std::stringstream ss(std::stringstream::out);
   if(mUseCodeInFilename) {
      std::string locationCode = locations[iKey.location].getCode();
      ss << getDataDirectory() << locationCode << "_" << localVariable << getFileExtension();
   }
   else {
      int locationNum = locations[iKey.location].getId();
      ss << getDataDirectory() << locationNum << "_" << localVariable << getFileExtension();
   }
   return ss.str();
}
bool InputFlatObs::getDatesCore(std::vector<int>& iDates) const {
   std::string filename = getSampleFilename();
   std::ifstream ifs(filename.c_str(), std::ifstream::in);

   // Loop over all rows
   std::set<int> dates;
   while(ifs.good()) {
      char line[10000];
      ifs.getline(line, 10000, '\n');
      if(ifs.good() && line[0] != '#') {
         std::stringstream ss(line);
         int date;
         ss >> date;
         dates.insert(date);
      }
   }
   iDates = std::vector<int>(dates.begin(), dates.end());
   ifs.close();
}
