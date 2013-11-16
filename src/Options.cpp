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
void Options::parseTag(const std::string& iTag) {
   std::stringstream ss(iTag);
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

void Options::setTag(const char* iTag) {
   parseTag(std::string(iTag));
}
void Options::setTag(const std::string& iTag) {
   parseTag(iTag);
}
void loadFile(const std::string& iFilename) {

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

bool Options::getOptionString(const std::string& iKey, std::string& iOptionString) const {
   std::map<std::string,std::string>::const_iterator it = mMap.find(iKey);
   if(it == mMap.end())
      return false;
   std::stringstream ss;
   ss << iKey << "=" << it->second;
   iOptionString = ss.str();
   return true;
}
bool Options::getRequiredOptionString(const std::string& iKey, std::string& iOptionString) const {
   bool found = getOptionString(iKey, iOptionString);
   if(!found) {
      std::stringstream ss;
      ss << "Required tag: " << mTag << "has missing key: " << iKey;
      Global::logger->write(ss.str(), Logger::error);
      return false;
   }
   return true;
}

bool Options::hasValue(const std::string& iKey) const {
   std::map<std::string,std::string>::iterator it = mMap.find(iKey);
   return(it != mMap.end());
}
bool Options::hasValues(const std::string& iKey) const {
   return hasValue(iKey) && isVector(iKey);
}
