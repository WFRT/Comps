#ifndef OBS_SELECTOR_NEAREST_H
#define OBS_SELECTOR_NEAREST_H
#include "ObsSelector.h"

//! Chooses the closest observation
class ObsSelectorNearest : public ObsSelector {
   public:
      ObsSelectorNearest(const Options& iOptions, const Data& iData);
      void select(std::vector<Obs>& iObs, const Location& iLocation, float iOffset) const;
};
#endif
