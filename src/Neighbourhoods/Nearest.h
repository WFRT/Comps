#ifndef NEIGHBOURHOOD_NEAREST_H
#define NEIGHBOURHOOD_NEAREST_H
#include "Neighbourhood.h"

//! Creates a neighbourhood using the nearest location. Curved earth distances are calculated.
class NeighbourhoodNearest : public Neighbourhood {
   public:
      NeighbourhoodNearest(const Options& iOptions);
   private:
      std::vector<Location> selectCore(const Input* iInput, const Location& iLocation) const;
      int mNum;
      bool mSkipIdentical;
};
#endif
