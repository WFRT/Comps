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
      std::stringstream ssi(temp, std::ios_base::ate | std::ios_base::in | std::ios_base::out);
      getline(ssi, key, '=');
      if(ssi.peek() == '[') {
         // The attribute is another scheme definitions: key=[scheme information]
         // Keep adding words until we find the ending ']'
         while(ss.good()) {
            ss >> temp;
            ssi << " " << temp;
            if(temp[temp.length()-1] == ']')
               break;
            if(!ss.good()) {
               std::stringstream ss;
               ss << "Error parsing '" << iOptions << "'. Did not find end ']'";
               Global::logger->write(ss.str(), Logger::error);
            }
         }
         getline(ssi, value, ']');
         value.append("]");
         mMap[key] = value;
      }
      else if(key != "") {
         getline(ssi, value, ' ');
         // Assume 'true' if no value specified
         if(value.length() == 0) {
            value = "1";
         }
         else if(value[0] == '#') {
            // Start of a comment, don't continue parsing
            return;
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
      if(values != "")
         ss << "=" << values;
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

void Options::addBoolOption(const std::string iKey) {
   std::stringstream ss;
   ss << iKey;
   addOptions(Options(ss.str()));
}

void Options::addOptions(const Options& iOptions) {
   parse(iOptions.toString());
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
   return hasValues(iKey) && !isVector(it->second);
}
bool Options::hasValues(const std::string& iKey) const {
   std::map<std::string,std::string>::iterator it = mMap.find(iKey);
   return(it != mMap.end());
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
      iTo.addOptions(Options(optionsString));
}
