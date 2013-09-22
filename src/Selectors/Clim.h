#ifndef SELECTOR_CLIM_H
#define SELECTOR_CLIM_H
#include "Selector.h"
//! Creates an ensemble by selecting past observations corresponding to a similar time of day
//! and date of year
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
            std::vector<Field>& rFields) const;
      int  mDayWindow;
      int  mHourWindow;
      int  mFutureBlackout;
      bool mAllowFutureValues;
      bool mAllowWrappedOffsets;
};
#endif
