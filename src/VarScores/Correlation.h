#ifndef VAR_SCORE_CORRELATION_H
#define VAR_SCORE_CORRELATION_H
#include "VarScore.h"

// The skill of the variable is the (negative) absolute correlation
class VarScoreCorrelation : public VarScore {
   public:
      VarScoreCorrelation(const Options& iOptions);
      float score(const std::vector<float>& iObs, const std::vector<float>& iForecasts) const;
};
#endif
