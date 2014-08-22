#include "Netcdf.h"
#include "../Parameters.h"
#include "../Data.h"

ParameterIoNetcdf::ParameterIoNetcdf(const Options& iOptions, std::string iConfiguration, const Data& iData) : ParameterIo(iOptions, iConfiguration, iData) {
   iOptions.check();
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
      int   componentsN = (int) mComponents.size();
      NcDim* dimPoolId = ncfile.get_dim("PoolId");
      NcDim* dimOffset = ncfile.get_dim("Offset");
      int offsetsN     = dimOffset->size();
      int poolIdN      = dimPoolId->size();

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

         int size = offsetsN*poolIdN*maxSize*indexN;
         float* paramArray = new float[size];
         float* paramSizes = new float[offsetsN*poolIdN*indexN];
         for(int i = 0; i < offsetsN*poolIdN*indexN; i++) {
            paramSizes[i] = 0;
         }

         // Get poolIds
         NcVar* varPoolId = ncfile.get_var("poolId");
         if(!varPoolId) {
            isCorrupt(filename, "Missing 'poolId' variable");
         }
         NcVar* varOffset = ncfile.get_var("offset");
         if(!varOffset) {
            isCorrupt(filename, "Missing 'offset' variable");
         }
         // PoolId
         int* locationArray = new int[poolIdN];
         long int id = 0;
         varPoolId->set_cur(&id);
         long count[1] = {poolIdN};
         varPoolId->get(locationArray, count);

         // Offsets
         float* offsetArray = new float[offsetsN];
         varOffset->set_cur(&id);
         count[0] = offsetsN;
         varOffset->get(offsetArray, count);

         // Get data
         NcVar* varParameter       = ncfile.get_var(componentName.c_str());
         NcVar* varParameterSize   = ncfile.get_var(getSizeName(componentName).c_str());
         if(varParameter) {
            assert(varParameterSize);
            long count[4] = {offsetsN,poolIdN,maxSize,indexN};
            varParameter->set_cur(0,0,0,0);
            varParameter->get(paramArray, count);
            varParameterSize->set_cur(0,0,0);
            long countSizes[3] = {offsetsN, poolIdN, indexN};
            varParameterSize->get(paramSizes, countSizes);
         }

         //mCache.add(iKey, Parameters()); // Add empty in case we don't find

         for(int o = 0; o < offsetsN; o++) {
            float offset = offsetArray[o];
            for(int f = 0; f < poolIdN; f++) {
               int locationId = locationArray[f];
               for(int k = 0; k < indexN; k++) {
                  int sizeIndex = o*poolIdN*indexN + f*indexN + k;
                  int currSize = paramSizes[sizeIndex];
                  std::vector<float> currParam;
                  //assert(currSize >= 0);
                  currParam.resize(currSize, Global::MV);
                  if(1 || f == iKey.mLocationId) {
                     for(int j = 0; j < currSize; j++) {
                        int i = o*poolIdN*maxSize*indexN +
                           f*maxSize*indexN +
                           j*indexN + 
                           k;
                        currParam[j] = paramArray[i];
                     }
                     Key::Par key(type, iKey.mDate, iKey.mInit, offset, locationId, iKey.mVariable, k);
                     mCache.add(key, currParam);
                  }
               }
            }
         }
         delete[] locationArray;
         delete[] offsetArray;
         delete[] paramArray;
         delete[] paramSizes;
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
void ParameterIoNetcdf::writeCore(const std::map<Key::Par,Parameters>& iParametersWrite) {
   // One file for each Date/Var
   // Find all configurations and variables
   std::map<std::string,std::set<std::string> > configVars;
   std::map<Key::DateInitVar, NcFile*> files; 
   std::set<Key::DateInitVar> keys;
   std::map<Key::Par, Parameters>::const_iterator it;
   std::map<Key::DateInitVar, std::set<int> > allPoolIds;
   std::map<Key::DateInitVar, std::set<float> > allOffsets;
   for(it = iParametersWrite.begin(); it != iParametersWrite.end(); it++) {
      Key::Par key0= it->first;
      std::string variable = key0.mVariable;
      int date = key0.mDate;
      int init = key0.mInit;

      Key::DateInitVar key(date, init, variable);
      keys.insert(key);
      allPoolIds[key].insert(key0.mLocationId);
      allOffsets[key].insert(key0.mOffset);
   }

   // Find maximum index sizes
   std::map<Component::Type, int> indexSizes;
   for(int i = 0; i < (int) mComponents.size(); i++) {
      Component::Type type = mComponents[i];
      indexSizes[type] = 0;
   }
   for(it = iParametersWrite.begin(); it != iParametersWrite.end(); it++) {
      Key::Par key0 = it->first;
      Component::Type type = key0.mType;
      indexSizes[type] = std::max(indexSizes[type], key0.mIndex+1);
   }

   // Find maximum parameter sizes
   std::map<Key::DateInitVar, std::map<Component::Type, int> > sizes; // configuration size;
   // Initialize sizes to 0
   std::set<Key::DateInitVar>::const_iterator it00;
   for(it00 = keys.begin(); it00 != keys.end(); it00++) {
      Key::DateInitVar key = *it00;
      for(int i = 0; i < (int) mComponents.size(); i++) {
         Component::Type type = mComponents[i];
         sizes[key][type] = 0;
      }
   }
   for(it = iParametersWrite.begin(); it != iParametersWrite.end(); it++) {
      Key::Par key0 = it->first;
      Parameters par = it->second;
      Component::Type type   = key0.mType;
      int             date   = key0.mDate;
      int             init   = key0.mInit;
      std::string variable = key0.mVariable;

      Key::DateInitVar key(date, init, variable);
      assert(sizes[key].find(type) != sizes[key].end());
      sizes[key][type] = std::max(sizes[key][type], par.size());
      indexSizes[type] = std::max(indexSizes[type], key0.mIndex);
   }

   // Set up files
   std::map<Key::DateInitVar,std::map<int,int> > poolIdMap; // File, poolId, index into array
   std::map<Key::DateInitVar,std::map<float,int> > offsetMap; // File, offset, index into array
   std::set<Key::DateInitVar>::const_iterator itFiles;
   for(itFiles = keys.begin(); itFiles != keys.end(); itFiles++) {
      Key::DateInitVar key = *itFiles;
      std::string filename = getFilename(key);

      NcFile* file = new NcFile(filename.c_str(), NcFile::Replace);
      files[key] = file;

      std::vector<float> offsets(allOffsets[key].begin(), allOffsets[key].end());
      std::vector<int>   poolIds(allPoolIds[key].begin(), allPoolIds[key].end());
      std::sort(poolIds.begin(), poolIds.end());
      std::sort(offsets.begin(), offsets.end());

      // Set up dimensions
      NcDim* dimOffset   = file->add_dim("Offset",   offsets.size());
      NcDim* dimPoolId   = file->add_dim("PoolId",   poolIds.size());
      NcVar* varOffset   = file->add_var("offset", ncFloat, dimOffset);
      NcVar* varPoolId   = file->add_var("poolId", ncInt, dimPoolId);
      writeVariable(varOffset, offsets);
      writeVariable(varPoolId, poolIds);

      // Set up dimensions for each component
      for(int i = 0; i < (int) mComponents.size(); i++) {
         std::string componentName = Component::getComponentName(mComponents[i]);

         int size = sizes[key][mComponents[i]];
         if(size > 0) {
            int indexSize = indexSizes[mComponents[i]];
            NcDim* dimIndex  = file->add_dim(getIndexName(componentName).c_str(), indexSize);
            NcDim* dimParameter = file->add_dim(componentName.c_str(), size);
            file->add_var(componentName.c_str(), ncFloat, dimOffset, dimPoolId, dimParameter, dimIndex);
            // Add variable indicating number of parameters
            file->add_var(getSizeName(componentName).c_str(), ncFloat, dimOffset, dimPoolId, dimIndex);
            NcVar* varSize = file->get_var(getSizeName(componentName).c_str());

            // Initialize parameter size to 0
            varSize->set_cur(0, 0, 0);
            int N = offsets.size()*poolIds.size()*indexSize;
            float* parametersN = new float[N];
            for(int i = 0; i < N; i++) {
               parametersN[i] = 0;
            }
            varSize->put(parametersN, offsets.size(),poolIds.size(),indexSize);
         }
      }

      for(int i = 0; i < poolIds.size(); i++) {
         int id = poolIds[i];
         poolIdMap[key][id] = i;
      }
      for(int i = 0; i < offsets.size(); i++) {
         float id = offsets[i];
         offsetMap[key][id] = i;
      }
   }

   // Insert data
   for(it = iParametersWrite.begin(); it != iParametersWrite.end(); it++) {
      Key::Par key0 = it->first;
      Parameters par = it->second;

      if(par.size() > 0) {
         Component::Type iType   = key0.mType;
         std::string componentName = Component::getComponentName(iType);
         int             date    = key0.mDate;
         int             init    = key0.mInit;
         float           offset  = key0.mOffset;
         int             poolId  = key0.mLocationId;
         int             index   = key0.mIndex;
         std::string variable = key0.mVariable;

         //std::cout << "Writing: " << offset << " " << iType << std::endl;

         Key::DateInitVar key(date, init, variable);
         NcFile* file = files[key];
         assert(file->is_valid());

         // Dimensions
         NcDim* dimParameter = file->get_dim(componentName.c_str());

         // Variables
         NcVar* varParameter     = file->get_var(componentName.c_str());
         NcVar* varParameterSize = file->get_var(getSizeName(componentName).c_str());

         // Get index into poolId and offset array
         int poolIndex = poolIdMap[key][poolId];
         int offsetIndex = offsetMap[key][offset];

         // Write data
         std::vector<float> paramVector = par.getAllParameters();
         int parametersN = (int) paramVector.size();
         float* parameters = new float[parametersN];
         for(int i = 0; i < parametersN; i++) {
            parameters[i] = paramVector[i];
         }
         varParameter->set_cur(offsetIndex, poolIndex, 0, index);
         varParameter->put(parameters, 1,1,parametersN, 1);

         assert(parametersN <= dimParameter->size());
         varParameterSize->set_cur(offsetIndex, poolIndex, index);
         //std::cout << "Size: " << parametersN << " Pos: " << idOffset <<  " " << location << " " << index << std::endl;
         varParameterSize->put(&parametersN, 1,1,1);

         //std::cout << "writing parameters: " << idComponent << " " << parameters[0] << std::endl;

         delete[] parameters;
      }
   }

   // Close files
   std::set<Key::DateInitVar>::const_iterator it0;
   for(it0 = keys.begin(); it0 != keys.end(); it0++) {
      Key::DateInitVar key = *it0;
      NcFile* file = files[key];
      file->close();
   }
}

std::string ParameterIoNetcdf::getFilename(const Key::Par& iKey) const {
   std::stringstream ss;
   ss << mBaseOutputDirectory << mRunDirectory << "/parameters/" << iKey.mDate << "_" << iKey.mInit << "_" << iKey.mVariable << "_" << mConfiguration << ".nc";
   return ss.str();
}
std::string ParameterIoNetcdf::getFilename(const Key::DateInitVar& iKey) const {
   std::stringstream ss;
   ss << mBaseOutputDirectory << mRunDirectory << "/parameters/" << iKey.mDate << "_" << iKey.mInit << "_" << iKey.mVariable << "_" << mConfiguration << ".nc";
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
