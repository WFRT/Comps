#ifndef INPUT_GRIB_H
#define INPUT_GRIB_H
#include "Input.h"
#include "../Cache.h"
#include "../Key.h"
#ifdef WITH_GRIB
#include <grib_api.h>
#endif

//! GRIB files version 2
class InputGrib : public Input {
   public:
      InputGrib(const Options& iOptions);
   private:
      void  getLocationsCore(std::vector<Location>& iLocations) const;
      void  getMembersCore(std::vector<Member>& iMembers) const;
      float getValueCore(const Key::Input& iKey) const;
      std::string getDefaultFileExtension() const;

      std::string getIndexFilename(const Key::Input iKey) const;
#ifdef WITH_GRIB
      static std::string getVariableName(grib_handle* iH);
      static float getOffset(grib_handle* iH);
      grib_index* getIndex(const Key::Input& iKey, const std::string& iLocalVariable) const;
#endif
      void optimizeCacheOptions();
      static void getVariableValues(const std::string& iVariable, std::string& iShortName, std::string& iLevelType, std::string& iLevel);
      // Options
      std::string mFilenamePrefix;
      std::string mFilenameMiddle;
      bool mMultiOffsetsPerFile;

      // Caching files that have been read
      mutable std::map<int,std::map<float, bool> > mMissingFiles; // date, offset

      void writeMissingToCache(const Key::Input& iKey) const; // Write MV to all locations for this key

      float mMV; // Missing value indicator in this grib file
};
#endif
