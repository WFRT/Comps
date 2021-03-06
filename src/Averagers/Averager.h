#ifndef AVERAGER_H
#define AVERAGER_H
#include "../Global.h"
#include "../Options.h"
#include "../Processor.h"
#include "../Ensemble.h"
#include "../Parameters.h"
#include "../Obs.h"
#include "../Distribution.h"

//! Reduces an ensemble to a deterministic forecast.
class Averager: public Processor {
   public:
      Averager(const Options& iOptions, const Data& iData);
      //! Compute the deterministic value
      virtual float average(const Distribution& iDist, const Parameters& iParameters) const = 0;
      static Averager* getScheme(const Options& iOptions, const Data& iData);
      static Averager* getScheme(const std::string& iTag, const Data& iData);
      //! Defaults to not updating parameters
      virtual void updateParameters(const std::vector<Distribution::ptr>& iDists,
            const std::vector<Obs>& iObs,
            Parameters& iParameters) const {};
};
#endif
