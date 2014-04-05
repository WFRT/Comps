#ifndef INTERPOLATOR_H
#define INTERPOLATOR_H
#include "../Global.h"
#include "../Component.h"
#include "../Options.h"

//! Represents schemes that take a set of (x,y) coordinates, and finds the y-value corresponding to
//! an x-value using interpolation. Provides both interpolated values, and slopes.
class Interpolator : public Component{
   public:
      Interpolator(const Options& iOptions);
      //! Finds interpolated value corresponding to x, based on x,y data points
      float interpolate(float x, const std::vector<float>& iX, const std::vector<float>& iY) const;
      float slope(float x, const std::vector<float>& iX, const std::vector<float>& iY) const;
      static Interpolator* getScheme(const Options& iOptions);
      static Interpolator* getScheme(const std::string& iTag);
      bool inDomain(float x, const std::vector<float>& iX) const;
      //! If true, then the inputs to interpolate and slope are guaranteed to be sorted
      virtual bool needsSorted() const {return true;};
      //! If true, then the inputs to interpolate and slope are guaranteed to all be valid
      virtual bool needsCleaned() const {return true;};

      static void sort(std::vector<float>& iX, std::vector<float>& iY);
      static void clean(std::vector<float>& iX, std::vector<float>& iY);
      static void sort(std::vector<float>& iX);
      static void clean(std::vector<float>& iX);
      //! Does the method guarantee monotonic interpolation if the interpolation points are monotonic?
      virtual bool isMonotonic() const {return false;};
   protected:
      // iX and iY are guaranteed to be the same size, and have 2 or more valid points
      virtual float interpolateCore(float x, const std::vector<float>& iX, const std::vector<float>& iY) const = 0;
      //! Numerically differentiates. Guarantees x is valid.
      virtual float slopeCore(float x, const std::vector<float>& iX, const std::vector<float>& iY) const;
      static float mDx;
      //! Default checks that x >= min(iX) and x <= max(iX)
      virtual bool inDomainCore(float x, const std::vector<float>& iX) const;
};
#endif
