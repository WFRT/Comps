#ifndef NEIGHBOURHOOD_UNION_H
#define NEIGHBOURHOOD_UNION_H
#include "Neighbourhood.h"

//! Creates a neighbourhood using all locations from multiple neighbourhoods
class NeighbourhoodUnion : public Neighbourhood {
   public:
      NeighbourhoodUnion(const Options& iOptions);
      ~NeighbourhoodUnion();
   private:
      std::vector<Location> selectCore(const Input* iInput, const Location& iLocation) const;
      std::vector<Neighbourhood*> mNeighbourhoods;
};
#endif
