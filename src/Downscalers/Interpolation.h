#ifndef DOWNSCALER_INTERPOLATION_H
#define DOWNSCALER_INTERPOLATION_H
#include "Downscaler.h"

class DownscalerInterpolation : public Downscaler {
   public:
      DownscalerInterpolation(const Options& iOptions, const Data& iData);
      float downscale(const Slice& iSlice,
            const std::string& iVariable,
            const Location& iLocation,
            const Parameters& iParameters) const;
      void updateParameters(const Slice& iSlice,
            const std::string& iVariable,
            const Location& iLocation,
            const Obs& iObs,
            Parameters& iParameters) const {};
      //bool needsTraining() const {return false;};
      void getDefaultParameters(Parameters& iParameters) const;
   private:
      int mNumPoints;
      int mOrder;
};
#endif
