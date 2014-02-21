#include "Step.h"
InterpolatorStep::InterpolatorStep(const Options& iOptions) : Interpolator(iOptions) {}
float InterpolatorStep::interpolateCore(float x, const std::vector<float>& iX, const std::vector<float>& iY) const {
   float y = Global::MV;

   if(x >= iX[iX.size()-1])
      return 1;
   if(x <= iX[0])
      return 0;

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

   if(x0 == x1) {
      y = (y0+y1)/2;
   }
   else {
      y = y0;
   }

   return y;
}

float InterpolatorStep::slopeCore(float x, const std::vector<float>& iX, const std::vector<float>& iY) const {
   float slope = Global::MV;

   if(x >= iX[iX.size()-1]-0.00001)
      return 0;
   if(x <= iX[0]+0.00001)
      return 0;

   int i0   = Global::getLowerIndex(x, iX);
   assert(Global::isValid(i0));
   assert(i0 >= 0);
   float x0 = iX[i0];

   if(x == x0)
      slope = mMaxSlope;
   else {
      slope = 0;
   }

   assert(slope >= 0);
   return slope;
}
