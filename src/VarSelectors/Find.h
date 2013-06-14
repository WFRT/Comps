#ifndef VAR_SELECTOR_FIND_H
#define VAR_SELECTOR_FIND_H
#include "VarSelector.h"

class VarSelectorFind : public VarSelector {
   public:
      VarSelectorFind(const Options& iOptions, const Data& iData);
      void getVariables(const Data& iData,
            int iInit,
            float iOffset,
            const Location& iLocation,
            std::vector<std::string>& iVariables) const;
   private:
      int mMaxVars;
      int mTrainingDays;
      // What is the maximum cross correlation between variables that are allowed to occur?
      static const float mMaxVarCrossCorrelation = 1;
      Options makeOptionsObs(std::string iVariable) const;

};
#endif
