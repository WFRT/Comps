#ifndef SELECTOR_PERISTENCE_H
#define SELECTOR_PERISTENCE_H
#include "Selector.h"

class SelectorPersistence : public Selector {
   public:
      //! Options:
      //! - useLatest (false): Use today's obs at 00 UTC. Otherwise use yesterday's obs at the same
      //!   time
      SelectorPersistence(const Options& iOptions, const Data& iData);
      bool isLocationDependent() const {return false;};
      bool isVariableDependent() const {return false;};
      bool needsTraining() const {return false;};
   private:
      void selectCore(int iDate,
            int iInit,
            float iOffset,
            const Location& iLocation,
            const std::string& iVariable,
            const Parameters& iParameters,
            std::vector<Slice>& rSlices) const;
      bool mUseLatest;
};
#endif
