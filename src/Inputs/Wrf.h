#ifndef INPUT_WRF_H
#define INPUT_WRF_H
#include "Input.h"
#include <netcdf.hh>

class DataKey;
class InputWrf : public Input {
   public:
      InputWrf(const Options& rOptions, const Data& iData);
   private:
      void   loadLocations() const;
      void   loadOffsets() const;
      void   loadMembers() const;
      mutable int mNumLocX;
      mutable int mNumLocY;

      float getValueCore(const Key::Input& iKey) const;
      std::string getFilename(const Key::Input& iKey) const;
      void optimizeCacheOptions();
      int getSubstring(const std::string iString, int iStart, int iLength) const;
};
#endif
