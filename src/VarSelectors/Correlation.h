#ifndef VAR_SELECTOR_CORRELATION_H
#define VAR_SELECTOR_CORRELATION_H
#include "VarSelector.h"

class VarSelectorCorrelation : public VarSelector {
   public:
      VarSelectorCorrelation(const Options& iOptions, const Data& iData);
      void getVariables(const Data& iData,
            int iInit,
            float iOffset,
            const Location& iLocation,
            std::vector<std::string>& iVariables) const;
   private:
      int mMaxVars;
      bool mUseAnomaly;
      bool mUseBias;
};
#endif
