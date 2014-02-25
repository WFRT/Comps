#include "Options.h"

Options::Options(const std::string& iOptions) {
   parse(iOptions);
}

void Options::parse(const std::string& iOptions) {
   std::stringstream ss(iOptions);
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

std::string Options::toString() const {
   std::stringstream ss;
   std::map<std::string,std::string>::iterator it = mMap.begin();
   while(it != mMap.end()) {
      std::string key = it->first;
      std::string values = it->second;
      ss << key;
      ss << "=";
      ss << values;
      it++;
      if(it != mMap.end())
         ss << " ";
   }
   return ss.str();
}


bool Options::isVector(const std::string& iString) {
   bool hasComma = std::string::npos != iString.find(',');
   bool hasColon = std::string::npos != iString.find(':');
   return (hasComma || hasColon);
}
void Options::addOption(const std::string& iOption) {
   parse(iOption);
}

void Options::addOption(const std::string iKey, std::string iValues) {
   std::stringstream ss;
   ss << iKey << "=" << iValues;
   addOption(ss.str());
}

void Options::addOptions(const Options& iOptions) {
   std::vector<std::string> keys = iOptions.getKeys();
   for(int i = 0; i < keys.size(); i++) {
      std::string key = keys[i];
      std::string optionString;
      iOptions.getOptionString(key, optionString);
      addOption(optionString);
   }
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
      ss << "Required tag: " << toString() << "has missing key: " << iKey;
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

std::vector<std::string> Options::getKeys() const {
   std::vector<std::string> keys;
   std::map<std::string,std::string>::const_iterator it;
   for(it = mMap.begin(); it != mMap.end(); it++) {
      keys.push_back(it->first);
   }
   return keys;
}

void Options::copyOption(std::string iKey, const Options& iFrom, Options& iTo) {
   std::string optionsString;
   iFrom.getOptionString(iKey, optionsString);
   if(optionsString != "")
      iTo.addOption(optionsString);
}
