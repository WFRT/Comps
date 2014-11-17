#ifndef NEIGHBOURHOOD_ALL_H
#define NEIGHBOURHOOD_ALL_H
#include "Neighbourhood.h"

//! Create a neighbourhood using all locations
class NeighbourhoodAll : public Neighbourhood {
   public:
      NeighbourhoodAll(const Options& iOptions);
   private:
      std::vector<Location> selectCore(const Input* iInput, const Location& iLocation) const;
};
#endif
