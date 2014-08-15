#include "Correlation.h"

VarScoreCorrelation::VarScoreCorrelation(const Options& iOptions) : VarScore(iOptions) {
}

float VarScoreCorrelation::score(const std::vector<float>& iObs, const std::vector<float>& iForecasts) const {
   float corr = Global::corr(iForecasts, iObs);
   return -fabs(corr);
}
