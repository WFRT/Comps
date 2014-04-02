#include "Grib.h"
#include "../Location.h"
#include "../Member.h"
#include "../Options.h"
#include <iomanip>
#include <cstring>

InputGrib::InputGrib(const Options& iOptions) :
      Input(iOptions),
      mMV(Global::MV),
      mFilenamePrefix(""),
      mFilenameMiddle(""),
      mMultiOffsetsPerFile(false) {
   iOptions.getValue("filenamePrefix", mFilenamePrefix);
   iOptions.getValue("filenameMiddle", mFilenameMiddle);
   //! Are data for all offsets located in a single file (as opposed to one file per offset)?
   iOptions.getValue("multiOffsetsPerFile", mMultiOffsetsPerFile);
   //! Missing value indicator used in this dataset
   iOptions.getValue("mv", mMV); 
   if(!mMultiOffsetsPerFile && mCacheOtherOffsets) {
      std::stringstream ss;
      ss << "InputGrib: Cannot cache other offsets in '" << getName() << "' dataset";
      Global::logger->write(ss.str(), Logger::warning);
   }
   if(mCacheOtherVariables) {
      std::stringstream ss;
      ss << "InputGrib: Cannot cache other variables in '" << getName() << "' dataset";
      Global::logger->write(ss.str(), Logger::warning);
   }

   if(iOptions.hasValue("useDateFolder") || iOptions.hasValue("useInitFolder")) {
      std::stringstream ss;
      ss << "InputGrib: '" << getName() << "' cannot specify useDateFolder or useInitFolder yet.";
      Global::logger->write(ss.str(), Logger::error);
   }

   init();
}

void InputGrib::getLocationsCore(std::vector<Location>& iLocations) const {
#ifdef WITH_GRIB
   std::string filename = getSampleFilename();
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
         Location loc(getName(), id, lat, lon);
         loc.setElev(elev);
         iLocations.push_back(loc);
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

void InputGrib::getMembersCore(std::vector<Member>& iMembers) const {
   iMembers.push_back(Member(getName(), 0, "", 0));
}

std::string InputGrib::getIndexFilename(const Key::Input iKey) const {
   std::stringstream ss(std::stringstream::out);
   int yyyymm = iKey.date / 100;
   if(mMultiOffsetsPerFile) {
      ss << getDataDirectory() << yyyymm << "/" << iKey.date << "/" << mFilenamePrefix << iKey.date << mFilenameMiddle << Input::getFileExtension();
   }
   else {
      ss << getDataDirectory() << yyyymm << "/" << iKey.date << "/" << mFilenamePrefix << iKey.date << mFilenameMiddle
         << std::setfill('0') << std::setw(3) << round(iKey.offset) << Input::getFileExtension();
   }
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

   std::string localVariable;
   bool found = getLocalVariableName(iKey.variable, localVariable);
   assert(found);

   int numLocations = Input::getNumLocations();

   std::string filename = getFilename(iKey);
   std::stringstream ss;
   ss << "InputGrib: Loading " << filename << " " << iKey.date << " " << iKey.offset << " " << iKey.location << " " << localVariable;
   Global::logger->write(ss.str(), Logger::message);
   bool foundVariable = false;
   FILE* fid = fopen(filename.c_str(),"r");
   float value = Global::MV;
   if(fid) {
      // GRIB File found
      int err = 0;
      grib_handle* h = NULL;

      // double s = Global::clock();

      // Try to use an index to read file as this is much faster. Fall back on just reading the 
      // GRIB file.
      grib_index* gribIndex = getIndex(iKey, localVariable);
      bool validIndex = (gribIndex != NULL);
      if(!validIndex) {
         std::stringstream ss;
         ss << "InputGrib: No index file available for " << filename;
         Global::logger->write(ss.str(), Logger::message);
      }

      int counter = 1;
      // Loop over available variables (in index or in file)
      while(1) {
         // Read message from file or index
         if(!validIndex) {
            h = grib_handle_new_from_file(0,fid,&err);
         }
         else {
            h = grib_handle_new_from_index(gribIndex,&err);
         }
         if(h == NULL)
            break; // No more messages to process

         std::string currVariable = getVariableName(h);
         std::stringstream ss;
         ss << "InputGrib: Reading message #" << counter << ": " << currVariable;
         Global::logger->write(ss.str(), Logger::message);

         // Check if the current variable is defined in the variable list
         int variableId;
         found = getVariableIdFromLocalVariable(currVariable, variableId);
         if(!found) {
            std::stringstream ss;
            ss << "InputGrib: Found variable " << currVariable << " in " << filename << " but this is not mapped to any variable in namelist" << std::endl;
            Global::logger->write(ss.str(), Logger::message);
         }
         else {
            // Only read the current variable if necessary
            if(mCacheOtherVariables || currVariable == localVariable) {
               std::vector<float> currValues;
               currValues.resize(numLocations, Global::MV);
               int numValid = 0;

               // Check that the message has the right number of locations
               size_t N;
               GRIB_CHECK(grib_get_size(h,"values",&N),0);
               if(N == numLocations) {
                  foundVariable = foundVariable || (currVariable == localVariable);
                  double* arr = new double[N];

                  GRIB_CHECK(grib_get_double_array(h,"values",arr,&N),0);
                  currValues.assign(arr, arr + numLocations);
                  for(int i = 0; i < (int) currValues.size(); i++) {
                     if(currValues[i] == mMV)
                        currValues[i] = Global::MV;
                     else
                        numValid++;
                  }
                  std::stringstream ss;
                  ss << "InputGrib: Number of valid values: " << numValid;
                  Global::logger->write(ss.str(), Logger::message);
                  delete arr;
               }
               else {
                  std::stringstream ss;
                  ss << "GribInput: Discarding variable " << currVariable << " in " << filename
                     << " because it has incorrect number of locations";
                  Global::logger->write(ss.str(), Logger::debug);
               }

               Key::Input key = iKey;
               key.offset = getOffset(h);
               // Cache values
               for(int i = 0; i < numLocations; i++) {
                  key.location = i;
                  key.variable = variableId;
                  if(key.location == iKey.location && currVariable == localVariable) {
                     // Found the value
                     value = currValues[i];
                  }
                  if(mCacheOtherLocations || key.location == iKey.location) {
                     //if(currVariable == localVariable)
                     //   std::cout << currValues[i] << std::endl;
                     Input::addToCache(key, currValues[i]);
                  }
               }
            }
         }
         if(h) {
            grib_handle_delete(h);
         }

         // Quit reading file if we have found the variable we need
         if(!mCacheOtherVariables && (currVariable == localVariable)) {
            break;
         }
         counter++;
      }
      if(!foundVariable) {
         // File was there, but couldn't find variable
         std::stringstream ss;
         ss << "InputGrib: Could not find variable " << localVariable << " in " << filename;
         Global::logger->write(ss.str(), Logger::warning);
         writeMissingToCache(iKey);
      }
      if(validIndex) {
         grib_index_delete(gribIndex);
      }

      // double e = Global::clock();
      //std::cout << "Grib read time: " << e - s << " seconds" << std::endl;

      fclose(fid);
      return value;
   }
   else {
      // GRIB file not found
      std::stringstream ss;
      ss << "GribInput: File not found: " << filename;
      Global::logger->write(ss.str(), Logger::message);

      std::vector<float> currValues;
      currValues.resize(numLocations, Global::MV);
      for(int i = 0; i < numLocations; i++) {
         Key::Input key = iKey;
         key.location = i;
         if(mCacheOtherLocations || key.location == iKey.location)
            Input::addToCache(key, currValues[i]);
      }
      return Global::MV;
   }
#else
   return Global::MV;
#endif
}

#ifdef WITH_GRIB
std::string InputGrib::getVariableName(grib_handle* iH) {
   std::stringstream ss;
   char name1[512];
   char name2[512];
   char name3[512];
   char name4[512];
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
   int numLocations = Input::getNumLocations();
   for(int i = 0; i < numLocations; i++) {
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

#ifdef WITH_GRIB
grib_index* InputGrib::getIndex(const Key::Input& iKey, const std::string& iLocalVariable) const {
   std::string indexFilename = getIndexFilename(iKey);
   char* filename_index = new char[indexFilename.size()+1];
   std::strcpy(filename_index, indexFilename.c_str());
   FILE* fid0 = fopen(filename_index, "r");
   grib_index* gribIndex = NULL;

   if(fid0 != NULL) {
      int err = 0;
      gribIndex = grib_index_read(0, filename_index, &err);
      //std::cout << "reading" << std::endl;
      if(err != 0) {
         assert(gribIndex == NULL);
         std::stringstream ss;
         ss << "Input::Grib Error reading index file: " << indexFilename;
         Global::logger->write(ss.str(), Logger::warning);
      }
      fclose(fid0);
      delete filename_index;
   }

   if(gribIndex == NULL)
      return(NULL);

   std::string shortName;
   std::string levelType;
   std::string level;
   getVariableValues(iLocalVariable, shortName, levelType,level);

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
   return(gribIndex);
}
#endif

std::string InputGrib::getDefaultFileExtension() const {
   return "grb2";
}
