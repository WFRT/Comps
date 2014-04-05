#ifndef INTERPOLATOR_LINEAR_H
#define INTERPOLATOR_LINEAR_H
#include "../Global.h"
#include "Interpolator.h"

//! Connects each pair of coordinates by a straight line. Interpolation outside the domain of the
//! points gives the value of the nearest end point. In this case the slope is 0. When multiple points
//! have the same x-value but different y-values, the average of the smallest and largest y-value is
//! used when interpolating at this x-value.
class InterpolatorLinear: public Interpolator {
   public:
      InterpolatorLinear(const Options& iOptions);
      bool needsSorted()  const {return true;};
      bool needsCleaned() const {return true;};
      bool isMonotonic()  const {return true;};
   private:
      float interpolateCore(float x, const std::vector<float>& iX, const std::vector<float>& iY) const;
      float slopeCore(float x, const std::vector<float>& iX, const std::vector<float>& iY) const;
      static float mMaxSlope;
};
#endif
