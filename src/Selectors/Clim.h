#ifndef SELECTOR_CLIM_H
#define SELECTOR_CLIM_H
#include "Selector.h"
class SelectorClim : public Selector {
   public:
      SelectorClim(const Options& iOptions, const Data& iData);
      bool isLocationDependent() const {return false;};
      bool isVariableDependent() const {return false;};
      bool needsTraining() const {return false;};
      bool allowedToCheat() const {return mAllowFutureValues;};
   private:
      void selectCore(int iDate,
            int iInit,
            float iOffset,
            const Location& iLocation,
            const std::string& iVariable,
            const Parameters& iParameters,
            std::vector<Slice>& rSlices) const;
      int  mDayWindow;
      int  mHourWindow;
      int  mFutureBlackout;
      bool mAllowFutureValues;
      bool mAllowWrappedOffsets;
};
#endif
