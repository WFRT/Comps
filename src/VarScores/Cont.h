#ifndef VAR_SCORE_CONT_H
#define VAR_SCORE_CONT_H
#include "VarScore.h"

class VarScoreCont : public VarScore {
   public:
      VarScoreCont(const Options& iOptions);
      float score(const std::vector<float>& iObs, const std::vector<float>& iForecasts) const;
   private:
      std::vector<float> getPercentiles(const std::vector<float>& iForecasts) const;
};
#endif
