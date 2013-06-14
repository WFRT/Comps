#include "Namelist.h"

Namelist::Namelist(const std::string& rFilename) : mFolder("") {
   std::stringstream ss;
   ss << rFilename;
   mFilename = ss.str();
   init();
}
Namelist::Namelist(const std::string& iType, const std::string& iFolder) : mFolder(iFolder) {
   std::stringstream ss;
   ss << mBasePath << iFolder << "/" << iType << ".nl";
   mFilename = ss.str();
   init();
}
Namelist::Namelist(const std::string& rType, const std::string& rSubType, const std::string& rFilename) : mFolder("") {
   std::stringstream ss;
   ss << mBasePath << rType << "/" << rSubType << "/" << rFilename << ".nl";
   mFilename = ss.str();
   init();
}
/*
Namelist::Namelist(const std::string& iDataSetName, const std::string& iType) : mFolder("") {
   std::stringstream ss;
   ss << mBasePath << iDataSetName << "/" << iType << ".nl";
   mFilename = ss.str();
   init();
}
*/
std::string Namelist::mBasePath = "namelists/";
std::string Namelist::findLine(const std::string& iKey) const {
   std::string line = "";
   findLine(iKey, line);
   return line;
}
bool Namelist::findLine(const std::string& iKey, std::string& iLine) const {
   std::map<std::string,std::vector<std::string> >::iterator it = mMap.find(iKey);
   if(it != mMap.end()) {
      std::stringstream ss;
      // Add key
      if(mFolder != "" && mFolder != "default") {
         ss << "tag=" << mFolder << "." << iKey << " ";
      }
      else {
         ss << "tag=" << iKey << " ";
      }
      for(int i = 0; i < (int) (it->second).size(); i++) {
         ss << (it->second)[i] << " ";
      }
      iLine = ss.str();
      return true;
   }
   return false;
}
void Namelist::findValue(std::string iKey, int i, std::string& rValue) const {
   std::map<std::string,std::vector<std::string> >::iterator it = mMap.find(iKey);
   if(it != mMap.end()) {
      rValue = (it->second)[i];
   }
}
void Namelist::findValues(std::string iKey, std::vector<std::string>& rValues) const {
   std::map<std::string,std::vector<std::string> >::iterator it = mMap.find(iKey);
   if(it != mMap.end()) {
      rValues = it->second;
   }
}
void Namelist::findValuesToEnd(std::string iKey, int i, std::vector<std::string>& rValues) const {
   std::map<std::string,std::vector<std::string> >::iterator it = mMap.find(iKey);
   if(it != mMap.end()) {
      std::vector<std::string> values = it->second;
      for(int k = i; k < (int) values.size(); k++) {
         rValues.push_back(values[k]);
      }
   }
}
void Namelist::findValuesToEnd(std::string iKey, int i, std::string& rValues) const {
   std::map<std::string,std::vector<std::string> >::iterator it = mMap.find(iKey);
   std::stringstream ss;
   if(it != mMap.end()) {
      std::vector<std::string> values = it->second;
      for(int k = i; k < (int) values.size(); k++) {
         ss << values[k];
         ss << " ";
      }
   }
   rValues = ss.str();
}

void Namelist::init() {
   //
   std::ifstream ifs(mFilename.c_str(), std::ifstream::in);
   // Loop over each line
   while(ifs.good()) {
      char line[10000];
      ifs.getline(line, 10000, '\n');
      if(line[0] != '#' && line[0] != ' ') {
         std::stringstream ss(line);
         std::string key;
         std::vector<std::string> values;
         ss >> key;
         // Loop over each value
         while(ss.good()) {
            std::string value;
            ss >> value;
            values.push_back(value);
         }
         mMap[key] = values;
      }
   }
}
std::string Namelist::getDefaultLocation() {
   return "./namelists/";
}

void Namelist::getAllOptions(std::vector<Options>& rOptions) const {
   std::map<std::string,std::vector<std::string> >::iterator it;
   for(it = mMap.begin(); it != mMap.end(); it++) {
      rOptions.push_back(Options(findLine(it->first)));
   }
}
