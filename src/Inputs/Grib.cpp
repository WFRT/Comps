#include "Grib.h"
#include "../DataKey.h"
#include "../Location.h"
#include "../Member.h"
#include "../Options.h"
#include <iomanip>
#include <cstring>

InputGrib::InputGrib(const Options& iOptions, const Data& iData) :
      Input(iOptions, iData),
      mMV(Global::MV),
      mFilenamePrefix(""),
      mFilenameMiddle(""),
      mMultiOffsetsPerFile(false) {
   mFileExtension = "grb2";
   iOptions.getValue("filenamePrefix", mFilenamePrefix);
   iOptions.getValue("filenameMiddle", mFilenameMiddle);
   //! Are data for all offsets located in a single file (as opposed to one file per offset)?
   iOptions.getValue("multiOffsetsPerFile", mMultiOffsetsPerFile);
   //! Missing value indicator used in this dataset
   iOptions.getValue("mv", mMV); 
   if(!mMultiOffsetsPerFile && mCacheOtherOffsets) {
      std::stringstream ss;
      ss << "InputGrib: Cannot cache other offsets in '" << mName << "' dataset";
      Global::logger->write(ss.str(), Logger::warning);
   }
   if(mCacheOtherVariables) {
      std::stringstream ss;
      ss << "InputGrib: Cannot cache other variables in '" << mName << "' dataset";
      Global::logger->write(ss.str(), Logger::warning);
   }

   init();
}

void InputGrib::loadLocations() const {
#ifdef WITH_GRIB
   std::string filename = getConfigFilename("sample");
   FILE* fid = fopen(filename.c_str(),"r");
   if(!fid) {
      Global::logger->write("No sample file available for Grib", Logger::error);
   }
   else {
      int err = 0;
      grib_handle* h = grib_handle_new_from_file(0,fid,&err);
      if (h == NULL) {
         std::stringstream ss;
         ss << "Unable to create handle from file " << filename;
         Global::logger->write(ss.str(),Logger::error);
      }

      // Iterate over all locations
      grib_iterator* iter=grib_iterator_new(h,0,&err);                                     
      if (err != GRIB_SUCCESS) GRIB_CHECK(err,0);                       
      int id = 0;
      double lat, lon;
      double value;
      while(grib_iterator_next(iter,&lat,&lon,&value)) {   
         float elev = 0;//elevs[i];
         assert(lat <= 90 && lat >= -90);
         Location loc(mName, id, lat, lon, elev);
         mLocations.push_back(loc);
         id++;
      }
      grib_iterator_delete(iter);
      grib_handle_delete(h);
   }
   fclose(fid);
#else
    Global::logger->write("InputGrib: Program not compiled with GRIB", Logger::error);
#endif
}

void InputGrib::loadMembers() const {
   mMembers.push_back(Member(mName, 0, "", 0));
}

std::string InputGrib::getFilename(const Key::Input iKey, bool iIndex) const {
   std::stringstream ss(std::stringstream::out);
   int yyyymm = iKey.date / 100;
   if(mMultiOffsetsPerFile) {
      ss << mDataDirectory << yyyymm << "/" << iKey.date << "/" << mFilenamePrefix << iKey.date << mFilenameMiddle << ".grb2";
   }
   else {
      ss << mDataDirectory << yyyymm << "/" << iKey.date << "/" << mFilenamePrefix << iKey.date << mFilenameMiddle
         << std::setfill('0') << std::setw(3) << round(iKey.offset) << ".grb2";
   }
   if(iIndex)
      ss << ".gbx";
   return ss.str();
}

float InputGrib::getValueCore(const Key::Input& iKey) const {
#ifdef WITH_GRIB
   // Check that date hasn't been checked before
   std::map<int,std::map<float, bool> >::const_iterator it0 = mMissingFiles.find(iKey.date);
   if(it0 != mMissingFiles.end()) {
      // Date has missing files
      std::map<float, bool>::const_iterator it1 = it0->second.find(iKey.offset);
      if(it1 != it0->second.end()) {
         writeMissingToCache(iKey);
         return Global::MV;
      }
   }

   //int index = iKey.location;
   std::string localVariable = mId2LocalVariable[iKey.variable];

   std::string filename = getFilename(iKey);
   std::stringstream ss;
   ss << "InputGrib: Loading " << filename << " " << iKey.date << " " << iKey.offset << " " << iKey.location << " " << mId2LocalVariable[iKey.variable];
   Global::logger->write(ss.str(), Logger::message);
   //std::cout << "InputGrib: Loading " << filename << std::endl;
   FILE* fid = fopen(filename.c_str(),"r");
   if(fid) {
      // File found
      float value = Global::MV;
      int err = 0;
      grib_handle* h = NULL;

      bool found = false;
      double s = Global::clock();

      // Get index information
      std::string indexFilename = getFilename(iKey, true);
      char* filename_index = new char[indexFilename.size()+1];
      std::strcpy(filename_index, indexFilename.c_str());
      FILE* fid0 = fopen(filename_index, "r");
      grib_index* gribIndex = NULL;
      bool validIndex = false;

      if(fid0 != NULL) {
         gribIndex = grib_index_read(0, filename_index, &err);
         //std::cout << "reading" << std::endl;
         if(err == 0) {
            validIndex = true;
         }
         else {
            std::stringstream ss;
            ss << "Input::Grib Error reading index file: " << indexFilename;
            Global::logger->write(ss.str(), Logger::warning);
         }
         fclose(fid0);
         delete filename_index;

         if(validIndex) {
            //assert(gribIndex);
            // Reset grib index

            std::string shortName;
            std::string levelType;
            std::string level;
            getVariableValues(localVariable, shortName, levelType,level);

            char* shortNameChar = new char[shortName.size()+1];
            char* levelTypeChar = new char[levelType.size()+1];
            char* levelChar     = new char[level.size()+1];
            std::strcpy(shortNameChar, shortName.c_str());
            std::strcpy(levelTypeChar, levelType.c_str());
            std::strcpy(levelChar, level.c_str());
            // NOTE: If the grib file was partial, then the index file might not contain the key we
            // are searching for. In this case grib_handle_new_from_index will not create any handle
            // This is ok.
            grib_index_select_string(gribIndex, "levtype", levelTypeChar);
            grib_index_select_string(gribIndex, "levelist", levelChar);
            grib_index_select_string(gribIndex, "shortName", shortNameChar);
            delete shortNameChar;
            delete levelTypeChar;
            delete levelChar;
            if(gribIndex) {
               int counter = 0;
               //while((h= grib_handle_new_from_file(0,fid,&err))!= NULL) {
               //while((h= grib_handle_headers_only_new_from_file(0,fid,&err))!= NULL) {
               while((h= grib_handle_new_from_index(gribIndex,&err))!= NULL) {
                  //std::cout << counter << std::endl;
                  counter ++;
                  std::string variable = getVariableName(h);
                  std::stringstream ss;
                  ss << "InputGrib: Reading message: " << variable;
                  Global::logger->write(ss.str(), Logger::message);
                  std::map<std::string,int>::const_iterator it = mLocalVariable2Id.find(variable);
                  if(it == mLocalVariable2Id.end()) {
                     std::stringstream ss;
                     ss << "InputGrib: Found variable " << variable << " in " << filename << " but this is not mapped to any variable in namelist" << std::endl;
                     Global::logger->write(ss.str(), Logger::message);
                  }
                  else {
                     int variableId = it->second;

                     if(mCacheOtherVariables || variable == localVariable) {
                        std::vector<float> currValues;
                        currValues.resize(mLocations.size(), Global::MV);

                        // Check that the message has the right number of locations
                        size_t N;
                        GRIB_CHECK(grib_get_size(h,"values",&N),0);
                        if(N == mLocations.size()) {
                           found = (variable == localVariable);
                           double* arr = new double[N];

                           GRIB_CHECK(grib_get_double_array(h,"values",arr,&N),0);
                           currValues.assign(arr, arr + mLocations.size());
                           for(int i = 0; i < (int) currValues.size(); i++) {
                              if(currValues[i] == mMV)
                                 currValues[i] = Global::MV;
                           }
                           delete arr;
                        }
                        else {
                           std::stringstream ss;
                           ss << "GribInput: Discarding variable " << variable << " in " << filename
                              << " because it has incorrect number of locations";
                           Global::logger->write(ss.str(), Logger::debug);
                        }
                        Key::Input key = iKey;
                        key.offset = getOffset(h);
                        for(int i = 0; i < mLocations.size(); i++) {
                           key.location = i;
                           key.variable = variableId;
                           if(key.location == iKey.location) {
                              // Found the value
                              value = currValues[i];
                           }
                           if(mCacheOtherLocations || key.location == iKey.location)
                              Input::addToCache(key, currValues[i]);
                        }
                     }
                  }
                  if(h) {
                     grib_handle_delete(h);
                  }

                  // Quit reading file if we have found the variable we need
                  if(!mCacheOtherVariables && found) {
                     break;
                  }
               }
               if(counter == 0) {
                  std::stringstream ss;
                  ss << "InputGrib: Could not find variable " << localVariable << " in " << filename;
                  Global::logger->write(ss.str(), Logger::message);
               }
            }
         }
         if(gribIndex) {
            //std::cout << "deleting" << std::endl;
            grib_index_delete(gribIndex);
         }
      }
      else {
         std::stringstream ss;
         ss << "InputGrib: No index file available for " << filename;
         mMissingFiles[iKey.date][iKey.offset] = true;
         // Maybe shouldn't be critical, since there will be no index file on missing dates
         Global::logger->write(ss.str(), Logger::message);
      }

      double e = Global::clock();
      //std::cout << "Grib read time: " << e - s << " seconds" << std::endl;
      if(!found) {
         // Most likely the file was there, but it was corrupt or empty
         writeMissingToCache(iKey);
      }

      fclose(fid);

      return value;
   }
   else {
      // Use missing data for files not found
      std::stringstream ss;
      ss << "GribInput: File not found: " << filename;
      Global::logger->write(ss.str(), Logger::message);

      std::vector<float> currValues;
      currValues.resize(mLocations.size(), Global::MV);
      for(int i = 0; i < mLocations.size(); i++) {
         Key::Input key = iKey;
         key.location = i;
         if(mCacheOtherLocations || key.location == iKey.location)
            Input::addToCache(key, currValues[i]);
      }
      return Global::MV;
   }
#endif
}

#ifdef WITH_GRIB
std::string InputGrib::getVariableName(grib_handle* iH) {
   std::stringstream ss;
   char name1[512];
   char name2[512];
   char name3[512];
   char name4[512];
   long n1;
   long int n2;
   long int n3;
   size_t len1 = 512;
   size_t len2 = 512;
   size_t len3 = 512;
   size_t len4 = 512;

   int err = grib_get_string(iH, "shortName", name3, &len3); 
   assert(err == 0);
   std::string shortName = std::string(name3);

   err = grib_get_string(iH, "typeOfLevel", name1, &len1); 
   assert(err == 0);
   std::string typeOfLevel = std::string(name1);

   err = grib_get_string(iH, "levtype", name2, &len2); 
   assert(err == 0);
   std::string levtype = std::string(name2);
   if(typeOfLevel == "nominalTop") {
      levtype = "ntop";
   }

   err = grib_get_string(iH, "levelist", name4, &len4); 
   std::string level = "undef";
   if(err != GRIB_NOT_FOUND) {
      level = std::string(name4);
   }

   ss << shortName << "_" << levtype << "_" << level;
   return ss.str();
}

float InputGrib::getOffset(grib_handle* iH) {
   double offset;
   int err = grib_get_double(iH, "stepRange", &offset); 
   assert(err == 0);
   return (float) offset;
}

#endif
bool InputGrib::getDatesCore(std::vector<int>& iDates) const {
   boost::filesystem::directory_iterator end_itr; // default construction yields past-the-end
   // Loop over all months
   for(boost::filesystem::directory_iterator itrMonth(mDataDirectory); itrMonth != end_itr; ++itrMonth) {
      if(boost::filesystem::is_directory(itrMonth->status())) {
         // Loop over all days
         for(boost::filesystem::directory_iterator itrDay(itrMonth->path().string()); itrDay != end_itr; ++itrDay) {
            if(boost::filesystem::is_directory(itrDay->status())) {
               std::string filename = boost::filesystem::basename(itrDay->path().string());
               std::stringstream ss;
               ss << filename;
               int date;
               ss >> date;

               // Add date if not already added
               std::vector<int>::iterator it = find(iDates.begin(), iDates.end(), date);
               if(it == iDates.end()) {
                  iDates.push_back(date);
               }
            }
         }
      }
   }
   return true;
}

void InputGrib::optimizeCacheOptions() {
   mCacheOtherVariables = false; //true; We can't cache the other variables because
   mCacheOtherLocations = true;
   mCacheOtherMembers = false;
   if(mMultiOffsetsPerFile)
      mCacheOtherOffsets = true;
   else
      mCacheOtherOffsets = false;
}

void InputGrib::writeMissingToCache(const Key::Input& iKey) const {
   // Most likely the file was there, but it was corrupt or empty
   for(int i = 0; i < mLocations.size(); i++) {
      Key::Input key = iKey;
      key.location = i;
      if(mCacheOtherLocations || key.location == iKey.location)
         Input::addToCache(key, Global::MV);
   }
}

void InputGrib::getVariableValues(const std::string& iVariable, std::string& iShortName, std::string& iLevelType, std::string& iLevel) {
   int pos1 = Global::MV;
   int pos2 = Global::MV;
   for(int i = 0; i < iVariable.size(); i++) {
      if(iVariable[i] == '_') {
         if(pos1 == Global::MV) {
            pos1 = i;
         }
         else {
            pos2 = i;
            break;
         }
      }
   }
   assert(pos1 != Global::MV);
   assert(pos2 != Global::MV);
   iShortName = iVariable.substr(0,pos1);
   assert(pos2-pos1-1 > 0);
   iLevelType = iVariable.substr(pos1+1,pos2-pos1-1);

   if(iLevelType == "ntop")
      iLevelType = "sfc";
   iLevel = iVariable.substr(pos2+1,iVariable.size()-pos1-1);
   /*
   std::stringstream ss;
   //ss << iVariable.substr(pos2+1,iVariable.size()-pos2-1
   if(pos2 == iVariable.size()-1) {
      iLevel = 0;//Global::MV;
   }
   else {
      iLevel = atoi(iVariable.substr(pos2+1, iVariable.size()-pos2-1).c_str());
   }
   */
}
