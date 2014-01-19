#ifndef OPTIONS_H
#define OPTIONS_H
#include "Global.h"
class Namelist;

/** Represents run parameters. Accesses namelists **/
class Options {
   public:
      Options(const std::string& iTag="");
      /** Load options for a specific key from a file **/
      Options(const std::string& iKey, const std::string& iFilename);
      void setTag(const char* iTag);
      void setTag(const std::string& iTag);
      /** Read options from file **/
      void loadFile(const std::string& iFilename);

      void addOption(const std::string& iOption);
      void addOption(const std::string iKey, std::string iValue);

      static void copyOption(std::string iKey, const Options& iFrom, Options& iTo);

      bool hasValue(const std::string& iKey) const;
      bool hasValues(const std::string& iKey) const;
      template <class T> void getRequiredValue(const std::string& iKey, T& iValue) const {
         bool status = getValue(iKey, iValue);
         if(!status) {
            std::stringstream ss;
            ss << "Required tag: " << mTag << "has missing key: " << iKey;
            Global::logger->write(ss.str(), Logger::error);
         }
      };
      template <class T> bool getValue(const std::string& iKey, T& iValue) const {
         std::map<std::string,std::string>::iterator it = mMap.find(iKey);
         if(it == mMap.end()) {
            std::stringstream ss;
            ss << "Tag: " << mTag << "has missing key: " << iKey;
            Global::logger->write(ss.str(), Logger::debug);
            // Cannot return Global::MV since T might be a string
            return false;
         }
         else {
            std::string tag = it->second;
            if(isVector(tag))
               return false;
            std::stringstream ss(tag);
            ss >> iValue;
            return true;
         }
      };
      template <class T> void getRequiredValues(const std::string& iKey, std::vector<T>& iValues) const {
         bool status = getValues(iKey, iValues);
         if(!status) {
            std::stringstream ss;
            ss << "Required tag: " << mTag << "has missing key: " << iKey;
            Global::logger->write(ss.str(), Logger::error);
         }
      }
      template <class T> bool getValues(const std::string& iKey, std::vector<T>& iValues) const {
         std::map<std::string,std::string>::iterator it = mMap.find(iKey);
         if(it == mMap.end()) {
            std::stringstream ss;
            ss << "Missing key: " << iKey << " in " << mTag;
            Global::logger->write(ss.str(), Logger::debug);
            return false;
         }
         else {
            // Attributes are organized as follows:
            // option=value1,value2,value3...
            // where values have three allowed formats:
            //    value
            //    start:finish
            //    start:increment:finish
            // Note that if start,increment, and finish are not integers, then Global::getInt() will
            // fail and crash.
            iValues.clear();
            std::string tag = it->second;
            std::stringstream ss(tag);
            // Parse collection of attributes separated by commas
            while(ss) {
               std::string betweenComma;
               if(!getline(ss, betweenComma, ','))
                  break; // We're at the end of the line

               // Extract value from specific between-comma string
               std::stringstream ss1(betweenComma);
               // Find values between colons
               std::string colon1;
               getline(ss1, colon1, ':');
               assert(ss1);

               std::string colon2;
               if(!getline(ss1, colon2, ':')) {
                  // No colons
                  std::stringstream ss2(colon1);
                  T value;
                  ss2 >> value;
                  iValues.push_back(value);
               }
               else {
                  float start  = Global::getFloat(colon1);
                  float finish = Global::getFloat(colon2);
                  float inc    = 1;
                  std::string colon3;
                  if(getline(ss1, colon3, ':')) {
                     // start:interval:end
                     inc = finish;
                     ss1 >> colon3;
                     finish = Global::getFloat(colon3);
                  }
                  // Check for errors
                  if(inc == 0 || (start < finish && inc < 0) || (start > finish && inc > 0) || 
                     (finish-start)/inc > Options::mMaxColonExpansion) {
                     iValues.clear();
                     return false;
                  }
                  float i = start;
                  // Don't use a for loop, since we don't know if we are going up or down
                  // therefore we don't know what condition to use
                  while(1) {
                     std::stringstream ss2;
                     // Use streams to convert from int to T
                     T value;
                     ss2 << i;
                     ss2 >> value;
                     iValues.push_back(value);
                     i = i + inc;

                     // Quit if we have gone past the domain
                     if(inc > 0 && i > finish)
                        break;
                     if(inc < 0 && i < finish)
                        break;
                  }
               }
            }
         }
         return true;
      }
      template <class T> void setValue(const std::string& key, const T& value) {
         std::stringstream ss;
         ss << value;
         mMap[key] = ss.str();
      };
      template <class T> void setValues(const std::string& key, const std::vector<T>& value) {
         std::stringstream ss;
         for(int i = 0; i < (int) value.size(); i++) {
            if(i > 0) {
               ss << ",";
            }
            ss << value[i];
         }
         mMap[key] = ss.str();
      };

      std::string getBinary() const;

      template <class T> void getAllValues(std::vector<T>& iValues) const {
         iValues.clear();
         std::map<std::string,std::string>::iterator it;
         for(it = mMap.begin(); it != mMap.end(); it++) {
            std::string tag = it->second;
            std::stringstream ss(tag);
            T value;
            // Parse CSV std::string
            while(ss) {
               std::string valueString;
               if(!getline(ss, valueString, ','))
                  break;
               // Extract value from specific std::string value
               std::stringstream ssi(valueString);
               ssi >> value;
               iValues.push_back(value);
            }
         }
      }
      std::string getTag() const;
      //! Get the full attribute string corresponding to iKey. Useful when copying from one set of
      //! options to anoter
      bool getOptionString(const std::string& iKey, std::string& iOptionString) const;
      bool getRequiredOptionString(const std::string& iKey, std::string& iOptionString) const;
   private:
      mutable std::map<std::string,std::string> mMap; //! map[tag] = value(s)
      //! Checks if a string represents a vector of values (i.e. has one or more commas)
      static bool isVector(const std::string& iString);
      void parseTag(const std::string& iTag);
      std::string mTag;
      static const int mMaxColonExpansion = 1e8; // Don't let colon syntax generate more than this many numbers
};
#endif

