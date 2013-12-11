#ifndef INPUT_RDA_NETCDF_H
#define INPUT_RDA_NETCDF_H
#include "Input.h"
#include "../Cache.h"
#include "../Key.h"
#include <netcdf.hh>

//! Retrives data from file format used by NCAR/CISL's research data archive (RDA). The global METAR
//! observation dataset 336.0 is supported. More info at http://rda.ucar.edu/datasets/ds336.0/.
class InputRdaNetcdf : public Input {
   public:
      InputRdaNetcdf(const Options& iOptions);
   private:
      float  getValueCore(const Key::Input& ikey) const;
      void   getLocationsCore(std::vector<Location>& iLocations) const;
      void   getOffsetsCore(std::vector<float>& iOffsets) const;
      void   getMembersCore(std::vector<Member>& iMembers) const;
      std::string getDefaultFileExtension() const;

      std::string getFilename(const Key::Input& iKey) const;
      mutable std::map<std::string, int> mLocationNames; // Name, id (internal ID is just 0,1,2,...)

      int    mTimeTolerance;
      static const float mMV = -99999;
      void optimizeCacheOptions();
};
#endif
