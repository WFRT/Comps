#include "NetcdfCf.h"
#include "../Location.h"
#include "../Member.h"
#include "../Options.h"
#include <iomanip>

InputNetcdfCf::InputNetcdfCf(const Options& iOptions) :
      Input(iOptions),
      mLatVar("Lat"), 
      mLonVar("Lon"),
      mLandUseVar(""),
      mTimeDim("Offset"),
      mEnsDim(""),
      mTimeDivisor(1),
      mHorizDims(std::vector<std::string>(1,"Location")) {

   //! Which variable stores latitudes
   iOptions.getValue("latVar", mLatVar);
   //! Which variable stores longitudes
   iOptions.getValue("lonVar", mLonVar);
   //! Which variable stores elevation
   iOptions.getValue("elevVar", mElevVar);
   //! Which variable stores land use index
   iOptions.getValue("landUse", mLandUseVar);
   //! Which dimension defines time
   iOptions.getValue("timeDim", mTimeDim);
   //! Which variable stores the reference time for the first offset
   iOptions.getValue("timeRef", mTimeRef);
   //! What should the times in the file be divided by to make them into hours
   iOptions.getValue("timeDivisor", mTimeDivisor);
   if(!iOptions.getValue("timeVar", mTimeVar)) {
      mTimeVar = mTimeDim;
   }
   //! What dimension names represent horizontal dimensions?
   iOptions.getValues("horizDims", mHorizDims);
   //! What dimension names represent vertical dimensions?
   iOptions.getValues("vertDims", mVertDims);
   //! What dimension name represents the ensemble member dimension?
   iOptions.getValue("ensDim", mEnsDim);
   if(iOptions.hasValue("locations")) {
      std::stringstream ss;
      ss << "InputNetcdfCf: '" << getName()
         << "' has 'locations' options. This is untested for NetcdfCf inputs" << std::endl;
      Global::logger->write(ss.str(), Logger::critical);
   }
   init();
   optimizeCacheOptions();
}

void InputNetcdfCf::getMembersCore(std::vector<Member>& iMembers) const {
   if(mEnsDim == "") {
      Input::getMembersCore(iMembers);
      if(iMembers.size() == 0)
         iMembers.push_back(Member(getName(), 0));
   }
   else {
      iMembers.clear();
      std::string filename = getSampleFilename();
      NcFile ncfile(filename.c_str());
      if(ncfile.is_valid()) {
         NcDim* dim = getDim(&ncfile, mEnsDim);
         long size = dim->size();
         for(int i = 0; i < size; i++) {
            iMembers.push_back(Member(getName(), i));
         }
      }
   }
}

void InputNetcdfCf::getLocationsCore(std::vector<Location>& iLocations) const {
   iLocations.clear();
   std::string filename = getLocationFilename();
   NcFile ncfile(filename.c_str());
   if(ncfile.is_valid()) {
      // Retrieve all horizontal dimensions
      std::vector<NcDim*> horizDims;
      std::vector<long> horizSizes;
      long totalSize = 1;
      int numHorizDims = mHorizDims.size();
      NcError q(NcError::silent_nonfatal);
      for(int i = 0; i < numHorizDims; i++) {
         NcDim* dim = getDim(&ncfile, mHorizDims[i]);
         horizDims.push_back(dim);

         long size = dim->size();
         horizSizes.push_back(size);
         totalSize *= size;
      }
      NcVar* ncLats = getRequiredVar(&ncfile, mLatVar);
      NcVar* ncLons = getRequiredVar(&ncfile, mLonVar);

      float* lats  = new float[totalSize];
      float* lons  = new float[totalSize];
      float* elevs = new float[totalSize];
      float* landUse = new float[totalSize];
      long* count = &horizSizes[0];
      ncLats->get(lats, count);
      ncLons->get(lons, count);

      if(mElevVar != "") {
         NcVar* ncElev = getRequiredVar(&ncfile, mElevVar);
         ncElev->get(elevs, count);
      }
      else {
         std::fill_n(elevs, totalSize, Global::MV);
      }

      if(mLandUseVar != "") {
         NcVar* ncLandUse = getRequiredVar(&ncfile, mLandUseVar);
         ncLandUse->get(landUse, count);
      }
      else {
         std::fill_n(landUse, totalSize, Global::MV);
      }

      for(int id = 0; id < totalSize; id++) {
         Location loc(getName(), id, lats[id], lons[id]);
         loc.setElev(elevs[id]);
         loc.setLandUse(landUse[id]);
         iLocations.push_back(loc);
      }

      if(horizSizes.size() == 2) {
         std::vector<float> gradX(horizSizes[0]*horizSizes[1],0);
         std::vector<float> gradY(horizSizes[0]*horizSizes[1],0);
         assert(iLocations.size() == horizSizes[0]*horizSizes[1]);
         int index = 0;
         const int N = horizSizes[1];
         for(int i = 0; i < horizSizes[0]; i++) { // Loop over latitudes
            for(int j = 0; j < horizSizes[1]; j++) { // Loop over longitudes
               Location loc = iLocations[index];
               float elev = elevs[index];
               int index1; // One off in N-S direction
               int index2; // One off in W-E direction
               if(i == 0)
                  index1 = getIndex(i+1, j, N);
               else
                  index1 = getIndex(i-1, j, N);
               if(j == 0)
                  index2 = getIndex(i, j+1, N);
               else
                  index2 = getIndex(i,j-1,N);

               assert(index == getIndex(i,j,N));
               assert(iLocations.size() > index1 && index1 >= 0);
               assert(iLocations.size() > index2 && index2 >= 0);
               Location loc1 = iLocations[index1];
               Location loc2 = iLocations[index2];
               float elev1 = loc1.getElev();
               float elev2 = loc2.getElev();

               float dx1 = Location::getDistance(loc.getLat(), loc.getLon(), loc.getLat(), loc1.getLon());
               float dx2 = Location::getDistance(loc.getLat(), loc.getLon(), loc.getLat(), loc2.getLon());
               float dy1 = Location::getDistance(loc.getLat(), loc.getLon(), loc1.getLat(), loc.getLon());
               float dy2 = Location::getDistance(loc.getLat(), loc.getLon(), loc2.getLat(), loc.getLon());
               int signx1 = loc.getLon() > loc1.getLon() ? 1 : -1;
               int signx2 = loc.getLon() > loc2.getLon() ? 1 : -1;
               int signy1 = loc.getLat() > loc1.getLat() ? 1 : -1;
               int signy2 = loc.getLat() > loc2.getLat() ? 1 : -1;
               if(dx1 > dx2)
                  gradX[index] =  signx1 * (elev - elev1) / dx1;
               else {
                  gradX[index] =  signx2 * (elev - elev2) / dx2;
               }
               if(dy1 > dy2)
                  gradY[index] =  signy1 * (elev - elev1) / dy1;
               else
                  gradY[index] =  signy2 * (elev - elev2) / dy2;

               iLocations[index].setGradientX(gradX[index]);
               iLocations[index].setGradientY(gradY[index]);
               index++;
            }
         }
      }

      delete[] lats;
      delete[] lons;
      delete[] elevs;
      delete[] landUse;
      ncfile.close();
   }
}

int InputNetcdfCf::getIndex(int i, int j, int jSize) const {
   return j + i*jSize;

}

void InputNetcdfCf::getOffsetsCore(std::vector<float>& iOffsets) const {
   iOffsets.clear();

   std::string filename = getSampleFilename();
   NcFile ncfile(filename.c_str());
   if(ncfile.is_valid()) {
      // Retrieve all horizontal dimensions
      NcError q(NcError::silent_nonfatal);
      NcDim* timeDim = getDim(&ncfile, mTimeDim);
      long numTimes = timeDim->size();

      // Retrive times from file
      NcVar* timeVar = getRequiredVar(&ncfile, mTimeVar);
      long count = numTimes;
      long* times = new long[numTimes];
      timeVar->get(times, &count);

      // Get reference time (time of first offset)
      long ref = 0;
      if(mTimeRef != "") {
         NcVar* refVar = getRequiredVar(&ncfile, mTimeRef);
         long count = 1;
         refVar->get(&ref, &count);
      }
      // Convert to COMPS times
      iOffsets.resize(numTimes, Global::MV);
      for(int i = 0; i < numTimes; i++) {
         long time = times[i]; 
         float offset = (time - ref)/mTimeDivisor;
         iOffsets[i] = offset;
      }
      delete[] times;
      ncfile.close();
   }
}

float InputNetcdfCf::getValueCore(const Key::Input& iKey) const {
   float returnValue = Global::MV;

   std::string filename = getFilename(iKey);
   NcFile ncfile(filename.c_str());
   std::string localVariable;
   bool found = getLocalVariableName(iKey.variable, localVariable);
   assert(found);
   int size = getNumLocations() * getNumOffsets();
   float* values = new float[size];

   // Pre-initialize all values to missing
   std::vector<float> offsets = getOffsets();
   int numLocations = getLocations().size();
   int numOffsets   = offsets.size();
   Key::Input key = iKey;
   for(int i = 0; i < numOffsets; i++) {
      key.offset = offsets[i];
      for(int j = 0; j < numLocations; j++) {
         key.location = j;
         Input::addToCache(key, Global::MV);
      }
   }

   // Retrive values if possible
   if(ncfile.is_valid() && localVariable != "") {
      // Retrieve all horizontal dimensions
      std::vector<NcDim*> horizDims;
      std::vector<long> horizSizes;
      long totalSize = 1;
      int numHorizDims = mHorizDims.size();
      NcError q(NcError::silent_nonfatal);
      for(int i = 0; i < numHorizDims; i++) {
         NcDim* dim = getDim(&ncfile, mHorizDims[i]);
         horizDims.push_back(dim);

         long size = dim->size();
         horizSizes.push_back(size);
         totalSize *= size;
      }

      assert(localVariable != "");
      NcDim* timeDim = getDim(&ncfile, mTimeDim);

      NcVar* ncvar = getVar(&ncfile, localVariable);
      if(ncvar != NULL) {
         int numDims = ncvar->num_dims();
         long* count = new long[numDims];
         std::vector<NcDim*> dims;
         // Loop over the variable's dimensions
         long totalSize = 1;
         int timeOrder = Global::MV; // Which dimension is the time in?
         std::vector<int> locationOrders; // Which dimensions are the locations in?
         for(int i = 0; i < numDims; i++) {
            NcDim* dim = ncvar->get_dim(i);
            dims.push_back(dim);
            // Determine if we need this dimension
            bool needDim = false;
            if(dim == timeDim) {
               needDim = true;
               timeOrder = i;
            }
            for(int j = 0; j < horizDims.size(); j++) {
               if(dim == horizDims[j]) {
                  needDim = true;
                  locationOrders.push_back(i);
               }
            }
            if(needDim)
               count[i] = dim->size();
            else
               count[i] = 1;
            totalSize *= count[i];
         }
         int timeSize = count[timeOrder];
         int locSize  = totalSize / timeSize;
         if(timeSize > numOffsets) {
            std::stringstream ss;
            ss << "The time dimension in '" << filename << "' (" << timeSize << ") "
               << "is larger than the sample size (" << numOffsets << "). Ignoring this file.";
            Global::logger->write(ss.str(), Logger::critical);
            return Global::MV;
         }
         if(locSize > numLocations) {
            std::stringstream ss;
            ss << "The location dimension in '" << filename << "' (" << locSize << ") "
               << "is larger than the sample size (" << numLocations << "). Ignoring this file.";
            Global::logger->write(ss.str(), Logger::critical);
            return Global::MV;
         }
         if(totalSize != size) {
            std::stringstream ss;
            ss << "File '" << filename << "' does not have the same dimensions ("
               << timeSize << "," << locSize << ") as the sample file ("
               << numOffsets << "," << numLocations << "). Assume last part of dimensions are missing";
            Global::logger->write(ss.str(), Logger::warning);
         }
         bool status = ncvar->get(values, count);
         assert(status);

         std::vector<int> countVector(count, count+numDims);

         assert(Global::isValid(timeOrder));
         assert(locationOrders.size() == horizDims.size());

         Key::Input key = iKey;

         std::vector<float> offsets = getOffsets();
         std::vector<int> indices;

         // Loop over all retrived values
         for(int i = 0; i < totalSize; i++) {
            float value = values[i];
            assert(returnValue != Global::NC);
            // Convert missing value
            if(value == NC_FILL_FLOAT)
               value = Global::MV;

            // Compute indices
            getIndices(i, countVector, indices);

            // Determine the key
            int offsetIndex = indices[timeOrder];
            key.offset = offsets[offsetIndex];

            if(locationOrders.size() == 1)
               key.location = indices[locationOrders[0]];
            else if(locationOrders.size() == 2) {
               // Row-major ordering of locations (last index varies fastest)
               int index0 = indices[locationOrders[0]];
               int index1 = indices[locationOrders[1]];
               int size1  = countVector[locationOrders[1]];
               key.location = index1 + index0 * size1;
               assert(key.location < numLocations);
            }

            Input::addToCache(key, value);

            assert(!std::isinf(value));
            if(key == iKey) {
               returnValue = value;
            }
         }
         if(std::isnan(returnValue)) {
            returnValue = Global::MV;
         }
      }
      else {
         std::stringstream ss;
         ss << "InputNetcdfCf: File " << filename << " does not contain local variable '"
            << localVariable << "'.";
         Global::logger->write(ss.str(), Logger::warning);
         // Handle missing file
         std::vector<float> offsets = getOffsets();
         int numLocations = getLocations().size();
         Key::Input key = iKey;
         for(int i = 0; i < numOffsets; i++) {
            key.offset = offsets[i];
            for(int j = 0; j < numLocations; j++) {
               key.location = j;
               Input::addToCache(key, Global::MV);
            }
         }
         return Global::MV;
      }
   }
   ncfile.close();

   return returnValue;
}

//! The optimal settings will depend on the retrival patterns (i.e. if all locations are queried, etc)
void InputNetcdfCf::optimizeCacheOptions() {
   mCacheOtherLocations = true;
   mCacheOtherOffsets   = true;
   mCacheOtherVariables = false;
   mCacheOtherMembers   = false;
}

std::string InputNetcdfCf::getDefaultFileExtension() const {
   return "nc";
}

NcDim* InputNetcdfCf::getDim(NcFile* iFile, std::string iName) const {
   NcDim* dim = iFile->get_dim(iName.c_str());
   if(dim == NULL) {
      std::stringstream ss;
      ss << "Dimension '" << iName << "' does not exist in dataset " << getName();
      Global::logger->write(ss.str(), Logger::error);
   }
   return dim;
}

NcVar* InputNetcdfCf::getRequiredVar(NcFile* iFile, std::string iName) const {
   NcVar* var = getVar(iFile, iName);
   if(var == NULL) {
      std::stringstream ss;
      ss << "Variable '" << iName << "' does not exist in dataset " << getName();
      Global::logger->write(ss.str(), Logger::error);
   }
   return var;
}
NcVar* InputNetcdfCf::getVar(NcFile* iFile, std::string iName) const {
   NcVar* var = iFile->get_var(iName.c_str());
   return var;
}

void InputNetcdfCf::getIndices(int i, const std::vector<int>& iCount, std::vector<int>& iIndices) const {
   // The last index changes fastest, the first slowest
   int numDims = iCount.size();
   iIndices.resize(numDims);
   int sizeSoFar = 1;
   for(int k = numDims-1; k >= 0; k--) {
      int index = floor(i / sizeSoFar);
      index     = index % iCount[k];
      iIndices[k] = index;
      sizeSoFar *= iCount[k];
   }
}
