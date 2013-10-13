#ifndef OBS_SELECTOR_OFFSET_H
#define OBS_SELECTOR_OFFSET_H
#include "ObsSelector.h"

//! Chooses obs within a certain offset range
class ObsSelectorOffset : public ObsSelector {
   public:
      ObsSelectorOffset(const Options& iOptions, const Data& iData);
      void select(std::vector<Obs>& iObs, const Location& iLocation, float iOffset) const;
   private:
      int mWindowLength;
      bool mMovingWindow;
};
#endif
