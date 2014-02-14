#ifndef INPUT_GRID_H
#define INPUT_GRID_H
#include "Input.h"
#include "../Cache.h"
#include "../Key.h"
#include <netcdf.hh>

//! Input with locations on a refular lat/long grid. It is only used to define locations,
//! and cannot contain any data. Creates one location for each combination of specified
//! latitudes and longitudes.
class InputGrid : public Input {
   public:
      InputGrid(const Options& iOptions);
   private:
      void  getLocationsCore(std::vector<Location>& iLocations) const;
      float getValueCore(const Key::Input& iKey) const {return Global::MV;};
      std::vector<float> mLats;
      std::vector<float> mLons;
};
#endif
