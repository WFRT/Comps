#ifndef INPUTNETCDF_H
#define INPUTNETCDF_H
#include "Input.h"
#include "../Cache.h"
#include "../Key.h"
#include <netcdf.hh>

class DataKey;
class InputNetcdf : public Input {
   public:
      InputNetcdf(const Options& rOptions, const Data& iData);
   private:
      void   loadLocations() const;
      void   loadOffsets() const;
      void   loadMembers() const;
      void   writeCore(const Input& iData, const Input& iDimensions, int iDate) const;

      float  getValueCore(const Key::Input& iKey) const;
      std::string getFilename(const Key::Input& iKey) const;
      int    mNOffsets;
      int    mNLocations;
      int    mNMembers;
      void   writeVariable(NcVar* iVariable, const std::vector<float>& iValues) const;
      void   optimizeCacheOptions();
};
#endif
