#ifndef OBS_SELECTOR_DISTANCE_H
#define OBS_SELECTOR_DISTANCE_H
#include "ObsSelector.h"

//! Include observations within a certain distance
class ObsSelectorDistance : public ObsSelector {
   public:
      ObsSelectorDistance(const Options& iOptions, const Data& iData);
      void select(std::vector<Obs>& iObs, const Location& iLocation, float iOffset) const;
   private:
      float mMaxDistance;
};
#endif
