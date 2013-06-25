#ifndef METRIC_H
#define METRIC_H
#include "../Global.h"
#include "../Obs.h"
#include "../Configurations/Configuration.h"
//! Schemes representing a verification metric for a single forecast/observation pair
class Metric : public Component {
   public:
      Metric(const Options& iOptions, const Data& iData);
      virtual float compute(int iDate,
                            int iInit,
                            float iOffset,
                            const Obs& rObs,
                            const Configuration& rConfiguration) const = 0;
      static Metric* getScheme(const Options& iOptions, const Data& iData);
      static Metric* getScheme(const std::string& iTag, const Data& iData);
      bool   isMandatory() const {return false;};
      virtual std::string getName() const = 0;
      virtual bool needsTraining() const {return false;};
   protected:
      //Forecaster::DetFlag  mDetFlag;
      //Forecaster::ProbFlag mProbFlag;
};
#endif
