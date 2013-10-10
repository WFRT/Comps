#include "Forecast.h"

Forecast::Forecast(const Deterministic& iDeterministic, const Ensemble& iEnsemble, const Distribution::ptr iDistribution) :
      mDeterministic(iDeterministic),
      mEnsemble(iEnsemble),
      mDistribution(iDistribution)
{
}
