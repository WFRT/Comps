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
            iValues.clear();
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
   private:
      mutable std::map<std::string,std::string> mMap; //! map[tag] = value(s)
      //! Checks if a string represents a vector of values (i.e. has one or more commas)
      static bool isVector(const std::string& iString);
      void parseTag(const std::string& iTag);
      std::string mTag;
};
#endif

