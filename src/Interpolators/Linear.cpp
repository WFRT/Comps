#include "Linear.h"
float InterpolatorLinear::mMaxSlope = 1e5;
InterpolatorLinear::InterpolatorLinear(const Options& iOptions) : Interpolator(iOptions) {}
float InterpolatorLinear::interpolateCore(float x, const std::vector<float>& iX, const std::vector<float>& iY) const {
   float y = Global::MV;

   if(x > iX[iX.size()-1])
      return iY[iX.size()-1];
   if(x < iX[0])
      return iY[0];

   int i0   = Global::getLowerIndex(x, iX);
   int i1   = Global::getUpperIndex(x, iX);
   assert(Global::isValid(i0));
   assert(i0 >= 0);
   assert(Global::isValid(i1));
   assert(i1 < iX.size());
   float x0 = iX[i0];
   float x1 = iX[i1];
   float y0 = iY[i0];
   float y1 = iY[i1];

   if(x0 == x1)
      y = (y0+y1)/2;
   else {
      assert(x1 >= x0);
      y = y0 + (y1 - y0) * (x - x0)/(x1 - x0);
   }

   return y;
}

float InterpolatorLinear::slopeCore(float x, const std::vector<float>& iX, const std::vector<float>& iY) const {
   float slope = Global::MV;

   if(x >= iX[iX.size()-1]-0.00001)
      return 0;
   if(x <= iX[0]+0.00001)
      return 0;

   int i0   = Global::getLowerIndex(x, iX);
   assert(Global::isValid(i0));
   assert(i0 >= 0);
   int i1   = Global::getUpperIndex(x, iX);
   assert(Global::isValid(i1));
   assert(i1 < iX.size());
   float x0 = iX[i0];
   float x1 = iX[i1];
   float y0 = iY[i0];
   float y1 = iY[i1];

   if(x0 == x1)
      slope = mMaxSlope;
   else {
      assert(x1 >= x0);
      assert(y1 >= y0);
      slope = (y1 - y0)/(x1 - x0);
   }

   assert(slope >= 0);
   return slope;
}
