#ifndef SELECTOR_CLIM_H
#define SELECTOR_CLIM_H
#include "Selector.h"
class SelectorClim : public Selector {
   public:
      //! Options:
      //! * dayLength: Find values within +- number of days
      //! * hourLength: Find values within +- number of hours
      //! - allowFutureValues (false): Allow dates in the future
      //! - futureBlackout (0): Future values must be this number of days into the future
      //! - allowWrappedOffsets (false): ?
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
