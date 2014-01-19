#ifndef INTERPOLATOR_STEP_H
#define INTERPOLATOR_STEP_H
#include "../Global.h"
#include "Interpolator.h"

//! Connects each pair of coordinates by a step function. The interpolation point immediately below
//! is used. The slope is 0 at all points, and a fixed large value at the interpolation points.
//! Interpolation outside the domain of the points gives the value of the nearest end point. When
//! multiple points have the same x-value but different y-values, the average of the smallest and
//! largest y-values is used when interpolating at this x-value.
class InterpolatorStep: public Interpolator {
   public:
      InterpolatorStep(const Options& iOptions, const Data& iData);
      bool needsSorted()  const {return true;};
      bool needsCleaned() const {return true;};
      bool isMonotonic()  const {return true;};
   private:
      float interpolateCore(float x, const std::vector<float>& iX, const std::vector<float>& iY) const;
      float slopeCore(float x, const std::vector<float>& iX, const std::vector<float>& iY) const;
      static const float mMaxSlope = 1e5;
};
#endif
