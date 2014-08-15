#ifndef VAR_SCORE_BINS_H
#define VAR_SCORE_BINS_H
#include "VarScore.h"

class VarScoreBins : public VarScore {
   public:
      VarScoreBins(const Options& iOptions);
      float score(const std::vector<float>& iObs, const std::vector<float>& iForecasts) const;
   private:
};
#endif
