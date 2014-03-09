#ifndef POOLER_ALL_H
#define POOLER_ALL_H
#include "Pooler.h"

//! Pools all observations locations together
class PoolerAll : public Pooler {
   public:
      PoolerAll(const Options& iOptions, const Data& iData);
   private:
      int findCore(const Location& iLocation) const;
};
#endif
