#ifndef DOWNSCALER_PERFORMANCE_H
#define DOWNSCALER_PERFORMANCE_H
#include "Downscaler.h"

class DetMetric;
class DownscalerPerformance : public Downscaler {
   public:
      DownscalerPerformance(const Options& iOptions, const Data& iData);
      ~DownscalerPerformance();
      float downscale(const Slice& iSlice,
            const std::string& iVariable,
            const Location& iLocation,
            const Parameters& iParameters) const;
      void updateParameters(const Slice& iSlice,
            const std::string& iVariable,
            const Location& iLocation,
            const Obs& iObs,
            Parameters& iParameters) const;
      void getDefaultParameters(Parameters& iParameters) const;
   private:
      int mNumPoints;
      DetMetric* mMetric;
};
#endif
