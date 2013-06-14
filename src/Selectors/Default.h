#ifndef SELECTOR_DEFAULT_H
#define SELECTOR_DEFAULT_H
#include "Selector.h"

/** Select only from one EPS */
class SelectorDefault : public Selector {
   public:
      SelectorDefault(const Options& rOptions, const Data& iData);
      void getDefaultParameters(Parameters& iParameters) const {};
      bool isLocationDependent() const {return false;};
      bool isVariableDependent() const {return false;};
      //bool isOffsetDependent() const {return false;};
      bool needsTraining() const {return false;};
      bool hasParameters() const {return false;};
   private:
      void selectCore(int iDate,
            int iInit,
            float iOffset,
            const Location& iLocation,
            const std::string& iVariable,
            const Parameters& iParameters,
            std::vector<Slice>& iSlices) const;
};
#endif
