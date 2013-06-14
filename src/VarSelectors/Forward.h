#ifndef VAR_SELECTOR_FORWARD_H
#define VAR_SELECTOR_FORWARD_H
#include "VarSelector.h"

class VarSelectorForward : public VarSelector {
   public:
      VarSelectorForward(const Options& iOptions, const Data& iData);
      void getVariables(const Data& iData,
            int iInit,
            float iOffset,
            const Location& iLocation,
            std::vector<std::string>& iVariables) const;
   private:
      int mMaxVars;
};
#endif
