#include "Linear.h"
InterpolatorLinear::InterpolatorLinear(const Options& iOptions, const Data& iData) : Interpolator(iOptions, iData) {}
float InterpolatorLinear::interpolateCore(float x, const std::vector<float>& iX, const std::vector<float>& iY) const {
   float y = Global::MV;

   if(x >= iX[iX.size()-1]-0.00001)
      return iY[iX.size()-1];
   if(x <= iX[0]+0.00001)
      return iY[0];

   int i0   = findIndex(x, iX)-1;
   int i1   = i0 + 1;
   assert(i1 < iX.size());
   assert(i0 >= 0);
   float x0 = iX[i0];
   float x1 = iX[i1];
   float y0 = iY[i0];
   float y1 = iY[i1];

   if(x0 == x1)
      y = (y0+y1)/2;
   else {
      assert(x1 >= x0);
      assert(y1 >= y0);
      y = y0 + (y1 - y0) * (x - x0)/(x1 - x0);
   }

   return y;
}

int InterpolatorLinear::findIndex(float P, const std::vector<float>& array) {
   if(array.size() == 0) {
      return Global::MV;
   }
   for(int i = 0; i < array.size(); i++) {
      // P < array[i]
      if(array[i] - P > -0.000001)
         return i;
   }
}

float InterpolatorLinear::slopeCore(float x, const std::vector<float>& iX, const std::vector<float>& iY) const {
   float slope = Global::MV;

   if(x >= iX[iX.size()-1]-0.00001)
      return 0;
   if(x <= iX[0]+0.00001)
      return 0;

   int i0   = findIndex(x, iX)-1;
   int i1   = i0 + 1;
   assert(i1 < iX.size());
   assert(i0 >= 0);
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
