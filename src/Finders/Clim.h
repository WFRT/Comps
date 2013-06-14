#ifndef FINDER_CLIM_H
#define FINDER_CLIM_H
#include "Finder.h"

class FinderClim : public Finder {
   public:
      FinderClim(const Options& iOptions, const Data& iData);
   protected:
      int findCore(const Location& iLocation) const;
};
#endif
