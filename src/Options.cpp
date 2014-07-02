#include "Options.h"

Options::Options(const std::string& iOptions) {
   parse(iOptions);
}

void Options::parse(const std::string& iOptions) {
   std::string rem = iOptions;
   while(rem != "") {

      // Remove leading whitespace
      while(rem[0] == ' ') {
         rem = rem.substr(1);
      }
      // At this, the next word is a key (with or without values)
      std::string key;
      std::string value;
      int nextSpace = rem.find(' ');
      int nextEqual = rem.find('=');

      if(rem[0] == '#') {
         // Comment, stop processing
         return;
      }
      else if(nextEqual == std::string::npos && nextSpace == std::string::npos) {
         // Boolean attribute at the end
         // e.g. doWork
         key = rem;
         value = "1";
         rem = "";
      }
      else if(nextEqual == std::string::npos) {
         // No more equal signs, but at least one space
         // Boolean attribute, only booleans after this
         // e.g. doWork doSomethingElse
         assert(nextSpace != std::string::npos);
         key = rem.substr(0, nextSpace);
         value = "1";
         rem = rem.substr(nextSpace);
      }
      else if(nextSpace == std::string::npos) {
         // At least one equal sign, no spaces
         // This is the last attribute
         // e.g. attr=value1,value2
         key = rem.substr(0, nextEqual);
         rem = rem.substr(nextEqual+1);
         value = rem;
         rem = "";
      }
      else if(nextSpace < nextEqual) {
         // More spaces and equal signs
         // Boolean attribute
         // e.g. doWork attr=...
         assert(nextSpace != std::string::npos);
         key = rem.substr(0, nextSpace);
         value = "1";
         rem = rem.substr(nextSpace);
      }
      else {
         // Several more non-boolean attributes
         // e.g. attr=... attr=...
         key = rem.substr(0, nextEqual);
         rem = rem.substr(nextEqual+1);
         value = "";
         // Get all values
         while(rem != "") {
            if(rem[0] == ' ') {
               break;
            }
            // [value],...
            // ,value...
            // value
            // value,...
            int nextSpace = rem.find(' ');
            int nextComma = rem.find(',');
            if(rem[0] == '[') {
               // e.g. [class=DownscalerNeighbourhood num=1]...
               int nextEnd  = rem.find(']');
               value.append(rem.substr(0,nextEnd+1));
               rem = rem.substr(nextEnd+1);
            }
            else if(rem[0] == ',') {
               // e.g. ,value... attr=...
               value.append(",");
               rem = rem.substr(1);
            }
            else if(nextSpace == std::string::npos) {
               // e.g. value1,...,[attr=value],... attr=...
               value.append(rem);
               rem = "";
            }
            else if(nextComma == std::string::npos || nextSpace < nextComma) {
               // Last value 
               // e.g. value1 attr=...
               value.append(rem.substr(0,nextSpace));
               rem = rem.substr(nextSpace);
            }
            else {
               // e.g. value1,... attr=...
               value.append(rem.substr(0,nextComma+1));
               rem = rem.substr(nextComma+1);
            }
         }
      }
      mMap[key] = value;
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

Options Options::getOption(const std::string& iKey) const {
   std::map<std::string,std::string>::const_iterator it = mMap.find(iKey);
   Options opt;
   if(it != mMap.end()) 
      opt.addOption(it->first, it->second);
   return opt;
}

void Options::copyOption(std::string iKey, const Options& iFrom, Options& iTo) {
   if(iFrom.hasValues(iKey))
      iTo.addOptions(iFrom.getOption(iKey));
}

bool Options::getValue(const std::string& iKey, std::string& iValue) const {
   std::map<std::string,std::string>::iterator it = mMap.find(iKey);
   if(it == mMap.end()) {
      std::stringstream ss;
      ss << "Missing key '" << iKey << "' missing in: " << toString();
      Global::logger->write(ss.str(), Logger::debug);
      return false;
   }
   else {
      std::string tag = it->second;
      iValue = tag;
      return true;
   }
};
bool Options::getValues(const std::string& iKey, std::vector<std::string>& iValues) const {
   iValues.clear();
   std::map<std::string,std::string>::iterator it = mMap.find(iKey);
   if(it == mMap.end()) {
      std::stringstream ss;
      ss << "Missing key '" << iKey << "' missing in: " << toString();
      Global::logger->write(ss.str(), Logger::debug);
      return false;
   }
   else {
      std::string tag = it->second;
      std::stringstream ss(tag);
      while(ss) {
         if(ss.peek() == '[') {
            std::string curr;
            getline(ss, curr, ']');
            curr.append("]");
            iValues.push_back(curr);
         }
         else {
            std::string curr;
            getline(ss, curr, ',');
            if(curr != "")
               iValues.push_back(curr);
         }
      }
      return true;
   }
};
