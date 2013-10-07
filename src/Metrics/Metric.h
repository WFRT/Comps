#ifndef METRIC_H
#define METRIC_H
#include "../Global.h"
#include "../Obs.h"
#include "../Configurations/Configuration.h"
//! Schemes representing a verification metric for a single forecast/observation pair
class Metric : public Component {
   public:
      void compute(const Obs& iForecast, const Forecast& iForecast, Score& iScore) const;
      static Metric* getScheme(const Options& iOptions, const Data& iData);
      static Metric* getScheme(const std::string& iTag, const Data& iData);
      virtual std::string getName() const = 0;
      bool   isMandatory() const {return false;};
      virtual bool needsTraining() const {return false;};
   protected:
      Metric(const Options& iOptions, const Data& iData);
      float computeCore(const Obs& iForecast, const Forecast& iForecast) const = 0;
      //! By default the metric will not be computed if the obs is invalid. Overwrite to change this.
      virtual bool needsValidObs()  const {return true;};
      //! By default the metric will not be computed if the fcst is invalid. Overwrite to change this.
      virtual bool needsValidFcst() const {return true;};
};
#endif
