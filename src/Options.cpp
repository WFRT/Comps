#include "Options.h"
#include "Namelist.h"

Options::Options(const std::string& iTag) : mTag(iTag) {
   parseTag(iTag);
}
Options::Options(const std::string& iKey, const std::string& iFilename) {
   Namelist nl(iFilename);
   mTag = nl.findLine(iKey);

   parseTag(mTag);
}
void Options::parseTag(const std::string& rTag) {
   std::stringstream ss(rTag);
   while(ss) {
      std::string key;
      std::string value;
      std::string temp;
      // Get a word
      ss >> temp;

      // Check if we have hit a comment
      if(temp[0] == '#')
          return;

      // Separate into key and value
      std::stringstream ssi(temp);
      getline(ssi, key, '=');
      if(key != "") {
         getline(ssi, value, ' ');
         // Assume 'true' if no value specified
         if(value == "") {
            value = "1";
         }
         mMap[key] = value;
      }
   }
}

void Options::setTag(const char* rTag) {
   parseTag(std::string(rTag));
}
void Options::setTag(const std::string& rTag) {
   parseTag(rTag);
}
void loadFile(const std::string& rFilename) {

}
std::string Options::getBinary() const {
   std::stringstream ss;
   std::map<std::string,std::string>::iterator it = mMap.begin();
   while(it != mMap.end()) {
      std::string key = it->first;
      std::string values = it->second;
      ss << key;
      ss << "=";
      ss << values;
      ss << " ";
      it++;
   }
   return ss.str();
}

bool Options::isVector(const std::string& iString) {
   bool hasComma = std::string::npos != iString.find(',');
   bool hasColon = std::string::npos != iString.find(':');
   return (hasComma || hasColon);
}

std::string Options::getTag() const {
   return mTag;
}

void Options::addOption(const std::string& iOption) {
   parseTag(iOption);
}
