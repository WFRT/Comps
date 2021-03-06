#ifndef INPUTNETCDF_H
#define INPUTNETCDF_H
#include "Input.h"
#include "../Data.h"
#include "../Cache.h"
#include "../Key.h"
#include <netcdf.hh>

//! Data in NetCDF format. Currently only a very specific format is supported:
//! Required dimensions:
//! - Offset (number of offsets)
//! - Location (number of locations)
//! - Member (number of ensemble members)
//! Required variables (dimension):
//! - Offset(Offset): List of offsets
//! - Lat(Location): Latitudes of locations
//! - Lon(Location): Longitudes of locations
//! - Id(Location): Ids of locations
//! Dataset variables:
//! - variableName(Offset, Location)
class InputNetcdf : public Input {
   public:
      InputNetcdf(const Options& iOptions);
   private:
      void   getLocationsCore(std::vector<Location>& iLocations) const;
      void   getOffsetsCore(std::vector<float>& iOffsets) const;
      void   getMembersCore(std::vector<Member>& iMembers) const;
      void  writeCore(const Data& iData, int iDate, int iInit, const std::vector<float>& iOffsets, const std::vector<Location>& iLocations, const std::vector<Member>& iMembers, const std::vector<std::string>& iVariables) const;
      std::string getDefaultFileExtension() const {return "nc";};

      float  getValueCore(const Key::Input& iKey) const;
      void   writeVariable(NcVar* iVariable, const std::vector<float>& iValues) const;
      void   optimizeCacheOptions();
      void   notifyInvalidSampleFile() const;
};
#endif
