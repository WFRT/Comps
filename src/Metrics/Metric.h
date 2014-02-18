#ifndef METRIC_H
#define METRIC_H
#include "../Global.h"
#include "../Obs.h"
#include "../Configurations/Configuration.h"
#include "../Distribution.h"
#include "../Component.h"
#include "../Score.h"
//! Schemes representing a verification metric for a single forecast/observation pair
class Metric : public Component {
   public:
      void compute(const Obs& iObs, const Distribution::ptr iForecast, Score& iScore) const;
      static Metric* getScheme(const Options& iOptions, const Data& iData);
      static Metric* getScheme(const std::string& iTag, const Data& iData);
      bool   isMandatory() const {return false;};
      bool   needsTraining() const {return false;};
   protected:
      Metric(const Options& iOptions, const Data& iData);
      virtual float computeCore(const Obs& iObs, const Distribution::ptr iForecast) const = 0;
      //! By default the metric will not be computed if the obs is invalid. Overwrite to change this.
      virtual bool needsValidObs()  const {return true;};
      //! By default the metric will not be computed if the fcst is invalid. Overwrite to change this.
      virtual bool needsValidFcst() const {return true;};
      const Data& mData;
};

class MetricBasic : public Metric {
   public:
      MetricBasic(const Options& iOptions, const Data& iData);
   protected:
      float computeCore(const Obs& iObs, const Distribution::ptr iForecast) const;
      virtual float computeCore(float iObs, float iForecast) const = 0;
   private:
      bool mUseMedian;
      bool mAnomaly;
};
#endif
