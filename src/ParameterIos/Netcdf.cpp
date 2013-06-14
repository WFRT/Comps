#include "Netcdf.h"
#include "../Parameters.h"
#include "../Finders/Finder.h"

ParameterIoNetcdf::ParameterIoNetcdf(const Options& iOptions, const Data& iData) : ParameterIo(iOptions, iData) {
}
bool ParameterIoNetcdf::readCore(const Key::Par& iKey, Parameters& iParameters) const {
   std::map<int,bool>::const_iterator it = mAvailableDates.find(iKey.mDate);
   // Check if we have already determined that the date is not available
   if(it != mAvailableDates.end()) {
      if(!it->second) {
         return false;
      }
   }
   std::string filename = getFilename(iKey);

   NcFile ncfile(filename.c_str());
   if(ncfile.is_valid()) {
      //std::cout << "Opening: " << filename << std::endl;
      int   offsetsN    = (int) mOffsets.size();
      int   componentsN = (int) mComponents.size();
      NcDim* dimFinder = ncfile.get_dim("Finder");
      int finderN       = dimFinder->size();

      // Read all components
      for(int c = 0; c < componentsN; c++) {
         Component::Type type = mComponents[c];
         std::string componentName = Component::getComponentName(type);

         NcError q(NcError::silent_nonfatal);
         NcDim* dimParameter = ncfile.get_dim(componentName.c_str());
         NcDim* dimIndex     = ncfile.get_dim(getIndexName(componentName).c_str());
         int maxSize = 0;
         if(dimParameter) {
            maxSize = dimParameter->size();
         }
         int indexN = 1;
         if(dimIndex) {
            indexN = dimIndex->size();
         }

         int size = offsetsN*finderN*maxSize*indexN;
         float* paramArray = new float[size];
         float* paramSizes = new float[offsetsN*finderN*indexN];
         for(int i = 0; i < offsetsN*finderN*indexN; i++) {
            paramSizes[i] = 0;
         }

         // Get finder data
         NcVar* varFinder = ncfile.get_var("finderId");
         if(!varFinder) {
            isCorrupt(filename, "Missing finderId variable");
         }
         int* finderArray = new int[finderN];
         long int id = 0;
         varFinder->set_cur(&id);
         long count[1] = {finderN};
         varFinder->get(finderArray, count);

         // Get data
         NcVar* varParameter       = ncfile.get_var(componentName.c_str());
         NcVar* varParameterSize   = ncfile.get_var(getSizeName(componentName).c_str());
         if(varParameter) {
            assert(varParameterSize);
            long count[4] = {offsetsN,finderN,maxSize,indexN};
            varParameter->set_cur(0,0,0,0);
            varParameter->get(paramArray, count);
            varParameterSize->set_cur(0,0,0);
            long countSizes[3] = {offsetsN, finderN, indexN};
            varParameterSize->get(paramSizes, countSizes);
         }

         //mCache.add(iKey, Parameters()); // Add empty in case we don't find

         for(int o = 0; o < offsetsN; o++) {
            float offset = mOffsets[o];
            for(int f = 0; f < finderN; f++) {
               int finderId = finderArray[f];
               for(int k = 0; k < indexN; k++) {
                  int sizeIndex = o*finderN*indexN + f*indexN + k;
                  int currSize = paramSizes[sizeIndex];
                  std::vector<float> currParam;
                  assert(currSize >= 0);
                  currParam.resize(currSize, Global::MV);
                  if(1 || f == iKey.mLocationId) {
                     for(int j = 0; j < currSize; j++) {
                        int i = o*finderN*maxSize*indexN +
                           f*maxSize*indexN +
                           j*indexN + 
                           k;
                        currParam[j] = paramArray[i];
                     }
                     Key::Par key(type, iKey.mDate, iKey.mInit, offset, finderId, iKey.mVariable, iKey.mConfigurationName, k);
                     mCache.add(key, currParam);
                  }
               }
            }
         }
         delete paramArray;
         delete paramSizes;
      }
      mAvailableDates[iKey.mDate] = true;
      if(!mCache.isCached(iKey)) {
         return false;
      }
      assert(mCache.isCached(iKey));
      ncfile.close();

      Parameters temp = mCache.get(iKey);
      if(temp.size() > 0 && !Global::isValid(temp[0])) {
         return false;
      }
      else {
         iParameters = temp;
         return true;
      }
   }
   else {
      ncfile.close();
      mAvailableDates[iKey.mDate] = false;
      return false;
   }
}
void ParameterIoNetcdf::writeCore() {

   // One file for each Date/Var/Config
   // Find all configurations and variables
   std::map<std::string,std::set<std::string> > configVars;
   std::map<Key::DateVarConfig, NcFile*> files; 
   std::set<Key::DateVarConfig> keys;
   std::map<Key::Par, Parameters>::const_iterator it;
   std::map<Key::DateVarConfig, std::set<int> > finders;
   for(it = mParametersWrite.begin(); it != mParametersWrite.end(); it++) {
      Key::Par key0= it->first;
      std::string variable = key0.mVariable;
      std::string name = key0.mConfigurationName;
      int date = key0.mDate;

      Key::DateVarConfig key(date, variable, name);
      keys.insert(key);
      finders[key].insert(key0.mLocationId);
   }

   // Find maximum index sizes
   std::map<Component::Type, int> indexSizes;
   for(int i = 0; i < (int) mComponents.size(); i++) {
      Component::Type type = mComponents[i];
      indexSizes[type] = 0;
   }
   for(it = mParametersWrite.begin(); it != mParametersWrite.end(); it++) {
      Key::Par key0 = it->first;
      Component::Type type = key0.mType;
      indexSizes[type] = std::max(indexSizes[type], key0.mIndex+1);
   }

   // Find maximum parameter sizes
   std::map<Key::DateVarConfig, std::map<Component::Type, int> > sizes; // configuration size;
   // Initialize sizes to 0
   std::set<Key::DateVarConfig>::const_iterator it00;
   for(it00 = keys.begin(); it00 != keys.end(); it00++) {
      Key::DateVarConfig key = *it00;
      for(int i = 0; i < (int) mComponents.size(); i++) {
         Component::Type type = mComponents[i];
         sizes[key][type] = 0;
      }
   }
   for(it = mParametersWrite.begin(); it != mParametersWrite.end(); it++) {
      Key::Par key0 = it->first;
      Parameters par = it->second;
      Component::Type type   = key0.mType;
      int             date   = key0.mDate;
      std::string variable = key0.mVariable;
      std::string name = key0.mConfigurationName;
      int             index = key0.mIndex;

      Key::DateVarConfig key(date, variable, name);
      assert(sizes[key].find(type) != sizes[key].end());
      sizes[key][type] = std::max(sizes[key][type], par.size());
      indexSizes[type] = std::max(indexSizes[type], key0.mIndex);
   }

   // Set up files
   std::map<Key::DateVarConfig,std::map<int,int> > finderMap; // File, finderId, index into array
   std::set<Key::DateVarConfig>::const_iterator itFiles;
   for(itFiles = keys.begin(); itFiles != keys.end(); itFiles++) {
      Key::DateVarConfig key = *itFiles;
      std::string filename = getFilename(key);

      NcFile* file = new NcFile(filename.c_str(), NcFile::Replace);
      files[key] = file;

      // Set up dimensions
      NcDim* dimOffset = file->add_dim("Offset", (int) mOffsetMap.size());
      NcDim* dimFinder = file->add_dim("Finder", (int) mFinder->size());
      for(int i = 0; i < (int) mComponents.size(); i++) {
         std::string componentName = Component::getComponentName(mComponents[i]);

         int size = sizes[key][mComponents[i]];
         if(size > 0) {
            //std::cout << "Name: " << getIndexName(componentName) << " " << indexSizes[mComponents[i]] << std::endl;
            NcDim* dimIndex  = file->add_dim(getIndexName(componentName).c_str(), indexSizes[mComponents[i]]);
            //for(int k = 0; k < indexSizes[mComponents[i]]; k++) {

               NcDim* dimParameter = file->add_dim(componentName.c_str(), size);
               file->add_var(componentName.c_str(), ncFloat, dimOffset, dimFinder, dimParameter, dimIndex);
               // Add variable indicating number of parameters
               file->add_var(getSizeName(componentName).c_str(), ncFloat, dimOffset, dimFinder, dimIndex);
            //}
         }
      }

      // Write finder Ids
      NcVar* varFinder = file->add_var("finderId", ncInt, dimFinder);
      std::map<Key::DateVarConfig, std::set<int> >::const_iterator it0 = finders.find(key);
      assert(it0 != finders.end());
      std::set<int> currFinders = finders[key];
      int counter = 0;
      int* finderArray = new int[currFinders.size()];
      std::set<int>::const_iterator itFinders;
      for(itFinders = currFinders.begin(); itFinders != currFinders.end(); itFinders++) {
         int id = *itFinders;
         finderArray[counter] = id;
         finderMap[key][id] = counter;
         counter++;
      }
      long int i = 0;
      varFinder->set_cur(&i);
      varFinder->put(finderArray, currFinders.size());
      delete finderArray;

   }

   // Insert data
   for(it = mParametersWrite.begin(); it != mParametersWrite.end(); it++) {
      Key::Par key0 = it->first;
      Parameters par = it->second;

      if(par.size() > 0) {
         Component::Type iType   = key0.mType;
         std::string componentName = Component::getComponentName(iType);
         int             date    = key0.mDate;
         float           offset  = key0.mOffset;
         int             finder  = key0.mLocationId;
         int             index   = key0.mIndex;
         std::string variable = key0.mVariable;
         std::string name = key0.mConfigurationName;

         //std::cout << "Writing: " << offset << " " << iType << std::endl;

         Key::DateVarConfig key(date, variable, name);
         NcFile* file = files[key];
         assert(file->is_valid());

         // Dimensions
         NcDim* dimParameter = file->get_dim(componentName.c_str());

         // Variables
         NcVar* varParameter     = file->get_var(componentName.c_str());
         NcVar* varParameterSize = file->get_var(getSizeName(componentName).c_str());

         // Get index into finderId array
         int finderId = finderMap[key][finder];

         // Write data
         int idOffset    = mOffsetMap[offset];

         std::vector<float> paramVector = par.getAllParameters();
         int parametersN = (int) paramVector.size();
         float* parameters = new float[parametersN];
         for(int i = 0; i < parametersN; i++) {
            parameters[i] = paramVector[i];
         }
         varParameter->set_cur(idOffset, finderId, 0, index);
         varParameter->put(parameters, 1,1,parametersN, 1);

         assert(parametersN <= dimParameter->size());
         varParameterSize->set_cur(idOffset, finderId, index);
         //std::cout << "Size: " << parametersN << " Pos: " << idOffset <<  " " << finder << " " << index << std::endl;
         varParameterSize->put(&parametersN, 1,1,1);

         //std::cout << "writing parameters: " << idComponent << " " << parameters[0] << std::endl;

         delete parameters;
      }
   }

   // Close files
   std::set<Key::DateVarConfig>::const_iterator it0;
   for(it0 = keys.begin(); it0 != keys.end(); it0++) {
      Key::DateVarConfig key = *it0;
      NcFile* file = files[key];
      file->close();
      delete file;
   }
}

std::string ParameterIoNetcdf::getFilename(const Key::Par& iKey) const {
   std::stringstream ss;
   ss << mBaseOutputDirectory << mRunDirectory << "/parameters/" << iKey.mDate << "_" << iKey.mConfigurationName << "_" << iKey.mVariable << ".nc";
   return ss.str();
}
std::string ParameterIoNetcdf::getFilename(const Key::DateVarConfig& iKey) const {
   std::stringstream ss;
   ss << mBaseOutputDirectory << mRunDirectory << "/parameters/" << iKey.mDate << "_" << iKey.mConfigurationName << "_" << iKey.mVariable << ".nc";
   return ss.str();
}

std::string ParameterIoNetcdf::getSizeName(const std::string& iName) {
   std::stringstream ss;
   ss << iName << "Size";
   return ss.str();
}

std::string ParameterIoNetcdf::getIndexName(const std::string& iName) {
   std::stringstream ss;
   ss << iName << "NumIndex";
   return ss.str();
}
void ParameterIoNetcdf::isCorrupt(std::string iFilename, std::string iMessage) {
   std::stringstream ss;
   ss << "ParameterIoNetcdf: Parameter file " << iFilename << " is corrupt. " << iMessage;
   Global::logger->write(ss.str(), Logger::error);
}
