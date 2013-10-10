#include "Forecast.h"

Forecast::Forecast(const Deterministic& iDeterministic, const Ensemble& iEnsemble, const Distribution::ptr iDistribution) :
   Entity(iEnsemble.getDate(), iEnsemble.getInit(), iEnsemble.getOffset(), iEnsemble.getLocation(), iEnsemble.getVariable()), 
      mDeterministic(iDeterministic),
      mEnsemble(iEnsemble),
      mDistribution(iDistribution)
{
   assert(mDeterministic.getDate() == mEnsemble.getDate());
   assert(mDeterministic.getInit() == mEnsemble.getInit());
   assert(mDeterministic.getOffset() == mEnsemble.getOffset());
   assert(mDeterministic.getLocation().getId() == mEnsemble.getLocation().getId());
   assert(mDeterministic.getVariable() == mEnsemble.getVariable());
   assert(mDistribution->getDate() == mEnsemble.getDate());
   assert(mDistribution->getInit() == mEnsemble.getInit());
   assert(mDistribution->getOffset() == mEnsemble.getOffset());
   assert(mDistribution->getLocation().getId() == mEnsemble.getLocation().getId());
   assert(mDistribution->getVariable() == mEnsemble.getVariable());
}
