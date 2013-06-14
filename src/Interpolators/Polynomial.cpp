#include "Polynomial.h"
#include "../Options.h"
#include "../Data.h"
InterpolatorPolynomial::InterpolatorPolynomial(const Options& iOptions, const Data& iData) : Interpolator(iOptions, iData) {
   Component::underDevelopment();
   if(!iOptions.getValue("order", mOrder)) {
      mOrder = 1;
   }
}
float InterpolatorPolynomial::interpolateCore(float x, const std::vector<float>& iX, const std::vector<float>& iY) const {
   /*
   // TODO: Deal with interpolation order
   int lowerIndex = -1;
   int N = (int) iX.size();
   for(int i = 0; i < N; i++) {
      if(iX[i] <= x) {
         lowerIndex = i;
      }
      else {
         break;
      }
   }
   if(lowerIndex == -1) {
      // x is below all points
      return Global::MV;
   }
   else if(lowerIndex == N-1 && iX[i] > x) {
      // x is above all points
      return Global::MV;
   }
   else {
      // x is inside the points (including the edges)
      if(x == iX[lowerIndex]) {
         // On top of a point
         if(lowerIndex > 0) {
            // On top of a point in the middle
            // Check if there are points to the left that are also the same
            if(iX[lowerIndex-1] == x) {
               // On top of two or more points
               // Find 
               int upperIndex = lowerIndex;
               for(int i = lowerIndex; i >= 0; i--) {
                  if(iCoordinates[i] == x) {
                     lowerIndex = i;
                  }
                  else {
                     break;
                  }
               }
               // Find the average of the lower and upper points we are on top of
               float y0 = iY[lowerIndex];
               float y1 = iY[upperIndex]; 
               return (y1 + y0)/2;
            }
            else {
               // On top of a single point
               return iY[lowerIndex];
            }
         }
         else {
            // On top of the left most point
            return iY[0];
         }
      }
      else {
         // In between two different points
         int upperIndex = lowerIndex + 1;
         float x0 = iX[lowerIndex];
         float x1 = iX[upperIndex];
         float y0 = iY[lowerIndex];
         float y1 = iY[upperIndex]; 

         return y0 + (y1 - y0)*(x - x0)/(x1 - x0);
      }
   }
   if(isValid(x) && isValid(x0) && isValid(x1) && isValid(y0) && isValid(y1)) {
      if(x0 == x1) {
         return (y1 + y0)/2;
      }
      else {
         assert(x >=x0);
         assert(x <=x1);
         assert(x1 != x0);
      }
   }
   else {
      return Global::MV;
   }
   */
   return 0;
}
