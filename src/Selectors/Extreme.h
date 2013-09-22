#ifndef SELECTOR_EXTREME_H
#define SELECTOR_EXTREME_H
#include "Selector.h"

/** Select only from one EPS */
class SelectorExtreme : public Selector {
   public:
      SelectorExtreme(const Options& rOptions, const Data& iData);
      void getExtremeParameters(Parameters& iParameters) const {};
      bool isLocationDependent() const {return false;};
      bool isVariableDependent() const {return false;};
      //bool isOffsetDependent() const {return false;};
      bool needsTraining() const {return false;};
      bool hasParameters() const {return false;};
   protected:
      void selectCore(int iDate,
            int iInit,
            float iOffset,
            const Location& iLocation,
            const std::string& iVariable,
            const Parameters& iParameters,
            std::vector<Field>& iFields) const;
};
#endif
