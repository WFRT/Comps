#ifndef FINDER_DEFAULT_H
#define FINDER_DEFAULT_H
#include "Finder.h"

//! Use one index for each output location
class FinderDefault : public Finder {
   public:
      FinderDefault(const Options& iOptions, const Data& iData);
   private:
      int findCore(const Location& iLocation) const;
      std::vector<Location> mLocations;
};
#endif
