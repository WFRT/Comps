#ifndef NEIGHBOURHOOD_INTERSECTION_H
#define NEIGHBOURHOOD_INTERSECTION_H
#include "Neighbourhood.h"

//! Creates a neighbourhood using only locations common in all neighbourhoods
class NeighbourhoodIntersection : public Neighbourhood {
   public:
      NeighbourhoodIntersection(const Options& iOptions);
      ~NeighbourhoodIntersection();
   private:
      std::vector<Location> selectCore(const Input* iInput, const Location& iLocation) const;
      std::vector<Neighbourhood*> mNeighbourhoods;
};
#endif
