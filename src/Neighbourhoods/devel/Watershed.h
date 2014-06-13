#ifndef NEIGHBOURHOOD_WATERSHED_H
#define NEIGHBOURHOOD_WATERSHED_H
#include "Neighbourhood.h"

//! Finds nearby locations that are in the same watershed
class NeighbourhoodWatershed : public Neighbourhood {
   public:
      NeighbourhoodWatershed(const Options& iOptions);
   private:
      void selectCore(const Input* iInput, const Location& iLocation, std::vector<Location>& iLocations) const;
      int mNum;
};
#endif

