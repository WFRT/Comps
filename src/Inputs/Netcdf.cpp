#include "Netcdf.h"
#include "../Location.h"
#include "../Member.h"
#include "../Options.h"

InputNetcdf::InputNetcdf(const Options& iOptions) :
      Input(iOptions) {
   if(iOptions.hasValue("locations")) {
      std::stringstream ss;
      ss << "InputNetcdf: '" << getName()
         << "' has 'locations' options. This is untested for Netcdf inputs" << std::endl;
      Global::logger->write(ss.str(), Logger::critical);
   }
   init();
}

void InputNetcdf::getLocationsCore(std::vector<Location>& iLocations) const {
   iLocations.clear();
   std::string filename = getLocationFilename();
   NcFile ncfile(filename.c_str());
   if(ncfile.is_valid()) {
      NcDim* ncLocationDim = ncfile.get_dim("Location");
      int numLocations = ncLocationDim->size();
      NcVar* ncLats = ncfile.get_var("Lat");
      NcVar* ncLons = ncfile.get_var("Lon");
      NcError q(NcError::silent_nonfatal);
      NcVar* ncIds  = ncfile.get_var("Id");
      bool hasId = false;
      if(ncIds)
         hasId = true;

      float* lats  = new float[numLocations];
      float* lons  = new float[numLocations];
      int* ids     = new int[numLocations];
      long count[1] = {numLocations};
      ncLats->get(lats, count);
      ncLons->get(lons, count);
      if(hasId)
         ncIds->get(ids, count);

      for(int i = 0; i < numLocations; i++) {
         int   id   = i;
         if(hasId)
            id = ids[i];
         float lat  = lats[i];
         float lon  = lons[i];
         float elev = 0;
         Location loc(getName(), id, lat, lon);
         loc.setElev(elev);
         iLocations.push_back(loc);
      }
      delete[] lats;
      delete[] lons;
      delete[] ids;
      ncfile.close();
   }
   else {
      notifyInvalidSampleFile();
   }
}

void InputNetcdf::getOffsetsCore(std::vector<float>& iOffsets) const {
   iOffsets.clear();
   std::string filename = getSampleFilename();
   NcFile ncfile(filename.c_str());
   if(ncfile.is_valid()) {
      NcDim* ncOffsetDim = ncfile.get_dim("Offset");
      int numOffsets   = ncOffsetDim->size();
      NcVar* ncOffsets = ncfile.get_var("Offset");
      float* offsets  = new float[numOffsets];
      //float* elevs = new float[numOffsets];
      long count[1] = {numOffsets};
      ncOffsets->get(offsets, count);
      for(int i = 0; i < numOffsets; i++) {
         iOffsets.push_back(offsets[i]);
      }
      delete[] offsets;
      ncfile.close();
   }
   else {
      notifyInvalidSampleFile();
   }
}

void InputNetcdf::getMembersCore(std::vector<Member>& iMembers) const {
   iMembers.clear();
   std::string filename = getSampleFilename();
   NcFile ncfile(filename.c_str());
   if(ncfile.is_valid()) {
      NcDim* ncMemberDim = ncfile.get_dim("Member");
      int numMembers   = ncMemberDim->size();
      NcVar* ncRes = ncfile.get_var("Resolution");
      float* res = new float[numMembers];

      long count[1] = {numMembers};
      ncRes->get(res, count);
      for(int i = 0; i < numMembers; i++) {
         Member member(getName(), res[i], "", i);
         iMembers.push_back(member);
      }
      delete[] res;
      ncfile.close();
   }
   else {
      notifyInvalidSampleFile();
   }
}

float InputNetcdf::getValueCore(const Key::Input& iKey) const {
   float returnValue = Global::MV;

   std::string filename = getFilename(iKey);

   int size = getNumOffsets()*getNumLocations()*getNumMembers();
   float* values = new float[size];
   NcFile ncfile(filename.c_str());
   std::string localVariable;
   bool found = getLocalVariableName(iKey.variable, localVariable);
   assert(found);
   if(!ncfile.is_valid() || localVariable == "") {
      for(int i = 0; i < size; i++) {
         values[i] = Global::MV;
      }
   }
   else {
      std::stringstream ss;
      ss << "InputNetcdf: Loading " << filename << " " << iKey;
      Global::logger->write(ss.str(), Logger::message);
      assert(localVariable != "");

      NcError q(NcError::silent_nonfatal);
      NcVar* ncvar = ncfile.get_var(localVariable.c_str());
      if(ncvar) {
         long count[3] = {getNumOffsets(),getNumLocations(),getNumMembers()};
         bool status = ncvar->get(values, count);
         assert(status);
      }
      else {
         std::stringstream ss;
         ss << "InputNetcdf: File " << filename << " does not contain local variable "
            << localVariable << ". Is the file corrupts?";
         Global::logger->write(ss.str(), Logger::warning);
         for(int i = 0; i < size; i++) {
            values[i] = Global::MV;
         }
      }
   }
   ncfile.close();

   std::vector<float>  offsets = getOffsets();
   std::vector<Member> members = getMembers();

   int oC = getOffsetIndex(iKey.offset);
   int oS = mCacheOtherOffsets ? 0 : oC;
   int oE = mCacheOtherOffsets ? getNumOffsets()-1 : oC;

   int lC = iKey.location;
   int lS = mCacheOtherLocations ? 0 : lC;
   int lE = mCacheOtherLocations ? getNumLocations()-1 : lC;

   int iC = iKey.member;
   int iS = mCacheOtherMembers ? 0 : iC;
   int iE = mCacheOtherMembers ? getNumMembers()-1 : iC;

   std::vector<float> vec;
   vec.assign(values, values + size);
   delete[] values;
   Key::Input key = iKey;
   if(0 && getName() == "rda336") {
      std::cout << "Date: " << iKey.date << " " << iKey.variable << " " << getName() << " " << mCacheOtherLocations << std::endl;
      std::cout << oC << " " << oS << " " << oE << std::endl;
      std::cout << lC << " " << lS << " " << lE << std::endl;
      std::cout << iC << " " << iS << " " << iE << std::endl;
   }

   for(int offsetIndex = oS; offsetIndex <= oE; offsetIndex++) {
      key.offset = offsets[offsetIndex];
      for(key.member = iS; key.member <= iE; key.member++) {
         for(key.location = lS; key.location <= lE; key.location++) {
            int index = offsetIndex*getNumLocations()*getNumMembers() + key.location*getNumMembers() + key.member;
            assert(index < vec.size());
            Input::addToCache(key, vec[index]);

            assert(!std::isinf(vec[index]));
            if(key == iKey) {
               returnValue = vec[index];
            }
         }
      }
   }
   if(std::isnan(returnValue)) {
      returnValue = Global::MV;
   }

   return returnValue;
}

void InputNetcdf::writeCore(const Input& iData, const Input& iDimensions, int iDate, int iInit) const {
   // Set up file
   Key::Input key;
   key.date = iDate;
   key.init = iInit;
   std::string filename = getFilename(key);
   Global::createDirectory(filename);

   NcFile ncfile(filename.c_str(), NcFile::Replace);

   if(!ncfile.is_valid()) {
      std::stringstream ss;
      ss << " InputNetcdf: Could not write file: " << filename;
      Global::logger->write(ss.str(), Logger::error);
   }

   // Get dimension sizes of 'from' Input.
   std::vector<float> offsets = iDimensions.getOffsets();
   std::vector<Location> locations = iDimensions.getLocations();
   std::vector<Member> members = iData.getMembers();
   std::vector<std::string> variablesDim = iDimensions.getVariables();
   std::vector<std::string> variablesData = iData.getVariables();

   std::vector<std::string> variables;
   for(int i = 0; i < (int) variablesDim.size(); i++) {
      for(int k = 0; k < (int) variablesData.size(); k++) {
         if(variablesDim[i] == variablesData[k])
            variables.push_back(variablesDim[i]);
      }
   }

   // Write dimensions
   NcDim* dimOffset   = ncfile.add_dim("Offset", (long) offsets.size());
   NcDim* dimLocation = ncfile.add_dim("Location", (long) locations.size());
   NcDim* dimMember   = ncfile.add_dim("Member", (long) members.size());

   // Write offsets
   NcVar* varOffsets  = ncfile.add_var("Offset", ncFloat, dimOffset);
   writeVariable(varOffsets, offsets);

   // Write lat/lons
   NcVar* varLats = ncfile.add_var("Lat", ncFloat, dimLocation);
   NcVar* varLons = ncfile.add_var("Lon", ncFloat, dimLocation);
   NcVar* varIds = ncfile.add_var("Id", ncInt, dimLocation);
   std::vector<float> lats;
   std::vector<float> lons;
   std::vector<float> ids;
   for(int i = 0; i < (int) locations.size(); i++) {
      lats.push_back(locations[i].getLat());
      lons.push_back(locations[i].getLon());
      ids.push_back(locations[i].getId());
   }
   writeVariable(varLats, lats);
   writeVariable(varLons, lons);
   writeVariable(varIds, ids);

   // Write resolution
   NcVar* varRes  = ncfile.add_var("Resolution", ncFloat, dimMember);
   std::vector<float> resolutions;
   for(int i = 0; i < (int) members.size(); i++) {
      resolutions.push_back(members[i].getResolution());
   }
   writeVariable(varRes, resolutions);

   // Preallocate
   int N = offsets.size() * locations.size() * members.size();
   float* values = new float[N];

   std::vector<std::string> localVariables;

   // Write each variable
   for(int v = 0; v < (int) variables.size(); v++) {
      for(int i = 0; i < N; i++)
         values[i] = Global::MV;

      std::string variable = variables[v];
      std::string localVariable;
      bool found = getLocalVariableName(variable, localVariable);
      if(!found) {
         std::stringstream ss;
         ss << "InputNetcdf::write: Do not know what to map " << variable << " to. Skipping.";
         Global::logger->write(ss.str(), Logger::message);
      }
      else if(std::find(localVariables.begin(), localVariables.end(), localVariable) != localVariables.end()) {
         std::cout << "Variable " << localVariable << " has already been written once. Discarding duplicate." << std::endl;
      }
      else {
         localVariables.push_back(localVariable);

         // Populate values;
         for(int l = 0; l < (int) locations.size(); l++) {
            int locationId = locations[l].getId();
            if(&iData != &iDimensions) {
               // If data and dimension sources are differet, find the nearest location
               std::vector<Location> nearestLocations;
               iData.getSurroundingLocations(locations[l], nearestLocations);
               assert(nearestLocations.size()>0);
               locationId = nearestLocations[0].getId();
            }

            for(int o = 0; o < (int) offsets.size(); o++) {
               float offset = offsets[o];
               for(int m = 0; m < (int) members.size(); m++) {
                  int memberId = members[m].getId();
                  // Don't use the scaled/shifted value
                  bool useScaled = false;
                  float value = iData.getValue(iDate, iInit, offset, locationId, memberId, variable, useScaled);
                  int index      = o*locations.size()*members.size() + l*members.size() + m;
                  assert(index < N);
                  values[index]  = value;
               }
            }
         }

         // Write values
         long count[3] = {offsets.size(), locations.size(), members.size()};
         NcVar* ncvar = ncfile.add_var(localVariable.c_str(), ncFloat, dimOffset, dimLocation, dimMember);
         ncvar->put(values, count);
      }
   }
   delete[] values;
   ncfile.close();
}

void InputNetcdf::writeVariable(NcVar* iVariable, const std::vector<float>& iValues) const {
   int N = (int) iValues.size();
   float* values = new float[N];
   for(int i = 0; i < (int) iValues.size(); i++) {
      values[i] = iValues[i];
   }
   long int pos = 0;
   iVariable->set_cur(&pos);
   iVariable->put(values, N);
   delete values;
}

//! The optimal settings will depend on the retrival patterns (i.e. if all locations are queried, etc)
void InputNetcdf::optimizeCacheOptions() {
   mCacheOtherLocations = true;
   mCacheOtherOffsets   = true;
   mCacheOtherVariables = false;
   mCacheOtherMembers   = true;
}

void InputNetcdf::notifyInvalidSampleFile() const {
   std::stringstream ss;
   ss << "InputNetcdf: invalid sample file: " << getSampleFilename();
   Global::logger->write(ss.str(), Logger::error);
}
