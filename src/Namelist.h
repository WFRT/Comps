#ifndef NAMELIST_H
#define NAMELIST_H
#include "Global.h"
#include "Options.h"

/**
 * Represents data stored in a namelist file
 */
class Namelist {
   public:
      Namelist(const std::string& rFilename);
      Namelist(const std::string& rType, const std::string& rSubType, const std::string& rFilename);
      //Namelist(const std::string& iDataSetName, const std::string& iType);
      Namelist(const std::string& iType, const std::string& iFolder);
      /**
       * Retrives matrix data from a whitespace separated file, but force the output to have at
       * least rRows rows and rColumns columns
       * @param rRows Minimum number of rows
       * @param rColumns Minimum columns
       * @param rValues Write data into this 2D vector
       * @param rMissingValue The missing value indicator to write to cells that are missing
       */
      template <class T> void getAllValues(std::vector<std::vector<T> >& rValues, int rRows, int rColumns, T rMissingValue) const {
         getAllValues(rValues);
         for(int r = 0; r < (int) rValues.size(); r++) {
            // Add any missing columns on the current row if needed
            if((int) rValues[r].size() != rColumns) {
               for(int i = (int) rValues[r].size(); i < rColumns; i++) {
                  rValues[i].push_back(rMissingValue);
               }
            }
         }
         // Add any missing rows if needed
         if((int) rValues.size() != rRows) {
            for(int r = (int) rValues.size(); r < rRows; r++) {
               std::vector<T> values;
               for(int i = 0; i < rColumns; i++) {
                  values.push_back(rMissingValue);
               }
               rValues.push_back(values);
            }
         }
      };
      /**
       * Retrives matrix data from a whitespace separated file
       * @param rValues Write data into this 2D vector
       */
      template <class T> void getAllValues(std::vector<std::vector<T> >& rValues) const {
         std::ifstream ifs(mFilename.c_str(), std::ifstream::in);
         // Loop over each line
         while(ifs.good()) {
            char line[10000];
            ifs.getline(line, 10000, '\n');
            if(ifs.good() && line[0] != '#') {
               std::stringstream ss(line);
               std::vector<T> values;
               // Loop over each value
               while(ss.good()) {
                  T value;
                  ss >> value;
                  values.push_back(value);
               }
               rValues.push_back(values);
            }
         }
      };

      template <class T> void getAllKeys(std::vector<T>& rKeys) const {
         std::ifstream ifs(mFilename.c_str(), std::ifstream::in);
         // Loop over each line
         while(ifs.good()) {
            char line[10000];
            ifs.getline(line, 10000, '\n');
            if(ifs.good() && line[0] != '#') {
               std::stringstream ss(line);
               T value;
               ss >> value;
               rKeys.push_back(value);
            }
         }
      };

      //template <class T> void getAllOptions(std::vector<Options>& rOptions) const {
      void getAllOptions(std::vector<Options>& rOptions) const;

      /**
       * Finds the full string associated with the given key
       */
      std::string findLine(const std::string& rKey) const;
      bool findLine(const std::string& rKey, std::string& iLine) const;

      /**
       * Finds the 'i'th value associated with the given key
       */
      void findValue(std::string rKey, int i, std::string& rValue) const;

      /**
       * Finds the string values associated with the given key
       * @param rValues Store string values in this vector
       */
      void findValues(std::string rKey, std::vector<std::string>& rValues) const;

      /**
       * Finds the string values associated with the given key
       * @param i Only write parameters from this index and onward
       * @param rValues Store string values in this vector
       */
      void findValuesToEnd(std::string rKey, int i, std::vector<std::string>& rValues) const;
      void findValuesToEnd(std::string rKey, int i, std::string& rValues) const;
      static std::string getDefaultLocation();

   private:
      std::string mFilename;
      static std::string mBasePath;
      static std::string mNamelistPath;
      mutable std::map<std::string,std::vector<std::string> > mMap;
      void init();
      std::string mFolder;
};
#endif

