#ifndef FINDER_DISTANCE_H
#define FINDER_DISTANCE_H
#include "Finder.h"

//! Finds nearest location
class FinderDistance : public Finder {
   public:
      FinderDistance(const Options& iOptions, const Data& iData);
   private:
      int findCore(const Location& iLocation) const;
      std::vector<float> mElevs;
      std::vector<float> mLats;
      std::vector<float> mLons;
};
#endif
