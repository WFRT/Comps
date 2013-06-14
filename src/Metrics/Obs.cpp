#include "Obs.h"
MetricObs::MetricObs(const Options& iOptions, const Data& iData) : Metric(iOptions, iData) {

}
float MetricObs::compute(int iDate,
            int iInit,
            float iOffset,
            const Obs& iObs,
            const Configuration& iConfiguration) const {
   return iObs.getValue();
}

std::string MetricObs::getName() const {
   return "Obs";
}
