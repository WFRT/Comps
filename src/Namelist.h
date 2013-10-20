#ifndef NAMELIST_H
#define NAMELIST_H
#include "Global.h"
#include "Options.h"

/**
 * Represents data stored in a namelist file
 */
class Namelist {
   public:
      Namelist(const std::string& iFilename);
      Namelist(const std::string& iType, const std::string& iSubType, const std::string& iFilename);
      //Namelist(const std::string& iDataSetName, const std::string& iType);
      Namelist(const std::string& iType, const std::string& iFolder);
      /**
       * Retrives matrix data from a whitespace separated file, but force the output to have at
       * least iRows rows and iColumns columns
       * @param iRows Minimum number of rows
       * @param iColumns Minimum columns
       * @param iValues Write data into this 2D vector
       * @param iMissingValue The missing value indicator to write to cells that are missing
       */
      template <class T> void getAllValues(std::vector<std::vector<T> >& iValues, int iRows, int iColumns, T iMissingValue) const {
         getAllValues(iValues);
         for(int r = 0; r < (int) iValues.size(); r++) {
            // Add any missing columns on the current row if needed
            if((int) iValues[r].size() != iColumns) {
               for(int i = (int) iValues[r].size(); i < iColumns; i++) {
                  iValues[i].push_back(iMissingValue);
               }
            }
         }
         // Add any missing rows if needed
         if((int) iValues.size() != iRows) {
            for(int r = (int) iValues.size(); r < iRows; r++) {
               std::vector<T> values;
               for(int i = 0; i < iColumns; i++) {
                  values.push_back(iMissingValue);
               }
               iValues.push_back(values);
            }
         }
      };
      /**
       * Retrives matrix data from a whitespace separated file
       * @param iValues Write data into this 2D vector
       */
      template <class T> void getAllValues(std::vector<std::vector<T> >& iValues) const {
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
               iValues.push_back(values);
            }
         }
      };

      template <class T> void getAllKeys(std::vector<T>& iKeys) const {
         std::ifstream ifs(mFilename.c_str(), std::ifstream::in);
         // Loop over each line
         while(ifs.good()) {
            char line[10000];
            ifs.getline(line, 10000, '\n');
            if(ifs.good() && line[0] != '#') {
               std::stringstream ss(line);
               T value;
               ss >> value;
               iKeys.push_back(value);
            }
         }
      };

      //template <class T> void getAllOptions(std::vector<Options>& iOptions) const {
      void getAllOptions(std::vector<Options>& iOptions) const;

      /**
       * Finds the full string associated with the given key
       */
      std::string findLine(const std::string& iKey) const;
      bool findLine(const std::string& iKey, std::string& iLine) const;

      /**
       * Finds the 'i'th value associated with the given key
       */
      void findValue(std::string iKey, int i, std::string& iValue) const;

      /**
       * Finds the string values associated with the given key
       * @param iValues Store string values in this vector
       */
      void findValues(std::string iKey, std::vector<std::string>& iValues) const;

      /**
       * Finds the string values associated with the given key
       * @param i Only write parameters from this index and onward
       * @param iValues Store string values in this vector
       */
      void findValuesToEnd(std::string iKey, int i, std::vector<std::string>& iValues) const;
      void findValuesToEnd(std::string iKey, int i, std::string& iValues) const;
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

