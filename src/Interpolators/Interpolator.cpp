#include "Interpolator.h"
#include "SchemesHeader.inc"

Interpolator::Interpolator(const Options& iOptions) : Component(iOptions) {}
#include "Schemes.inc"
float Interpolator::interpolate(float iX0, const std::vector<float>& iX, const std::vector<float>& iY) const {
   assert(iX.size() == iY.size());
   if(iX.size() == 0 || !Global::isValid(iX0))
      return Global::MV;
   if(iX.size() == 1)
      return iY[0];

   if(needsCleaned() || needsSorted()) {
      std::vector<float> x = iX;
      std::vector<float> y = iY;
      if(needsCleaned()) {
         Interpolator::clean(x, y);
         if(x.size() == 0)
            return Global::MV;
      }
      if(needsSorted()) {
         Interpolator::sort(x, y);
      }
      if(iX.size() == 0 || !Global::isValid(iX0))
         return Global::MV;
      if(iX.size() == 1)
         return iY[0];
      return interpolateCore(iX0, x, y);
   }
   else {
      return interpolateCore(iX0, iX, iY);
   }
}
void Interpolator::sort(std::vector<float>& iX, std::vector<float>& iY) {
   assert(iX.size() == iY.size());
   // Remove missing values
   std::vector<std::pair<float,float> > values;   
   for(int i = 0; i < (int) iX.size(); i++) {
      if(Global::isValid(iX[i]) && Global::isValid(iY[i])) {
         std::pair<float,float> p(iX[i], iY[i]);
         values.push_back(p);
      }
   }
   std::stable_sort(values.begin(), values.end(), Global::sort_pair_first<float,float>());
   iX.clear();
   iY.clear();
   for(int i = 0; i < (int) values.size(); i++) {
      iX.push_back(values[i].first);
      iY.push_back(values[i].second);
   }
}
void Interpolator::sort(std::vector<float>& iX) {
   // Remove missing values
   std::vector<float> values;   
   for(int i = 0; i < (int) iX.size(); i++) {
      if(Global::isValid(iX[i])) {
         values.push_back(iX[i]);
      }
   }
   std::sort(values.begin(), values.end());
   iX = values;
}
void Interpolator::clean(std::vector<float>& iX, std::vector<float>& iY) {
   std::vector<float> x;
   std::vector<float> y;
   x.reserve(iX.size());
   y.reserve(iY.size());
   for(int i = 0; i < iX.size(); i++) {
      if(Global::isValid(iX[i]) && Global::isValid(iY[i])) {
         x.push_back(iX[i]);
         y.push_back(iY[i]);
      }
   }
   iX = x;
   iY = y;
}
void Interpolator::clean(std::vector<float>& iX) {
   std::vector<float> x;
   x.reserve(iX.size());
   for(int i = 0; i < iX.size(); i++) {
      if(Global::isValid(iX[i])) {
         x.push_back(iX[i]);
      }
   }
   iX = x;
}

bool Interpolator::inDomain(float x, const std::vector<float>& iX) const {
   if(needsCleaned() || needsSorted()) {
      std::vector<float> array = iX;
      if(needsCleaned()) {
         Interpolator::clean(array);
         if(array.size() == 0)
            return Global::MV;
      }
      if(needsSorted()) {
         Interpolator::sort(array);
      }
      return inDomainCore(x, array);
   }
   else {
      return inDomainCore(x, iX);
   }
}

bool Interpolator::inDomainCore(float x, const std::vector<float>& iX) const {
   float min = *std::min_element(iX.begin(), iX.end());
   float max = *std::max_element(iX.begin(), iX.end());
   if(Global::isValid(min) && Global::isValid(max)) {
      return x >= min && x <= max;
   }
   else {
      return false;
   }
}

float Interpolator::slope(float x, const std::vector<float>& iX, const std::vector<float>& iY) const {
   if(!Global::isValid(x))
      return Global::MV;

   if(needsSorted()) {
      std::vector<float> xsorted = iX;
      std::vector<float> ysorted = iY;
      Interpolator::sort(xsorted, ysorted);
      return slopeCore(x, xsorted, ysorted);
   }
   else {
      return slopeCore(x, iX, iY);
   }

}

float Interpolator::slopeCore(float x, const std::vector<float>& iX, const std::vector<float>& iY) const {
   // Find slope above x
   float y1 = interpolate(x+mDx, iX, iY);
   float y0 = interpolate(x, iX, iY);
   if(!Global::isValid(y1)) {
      // Find slope below x
      y1 = y0;
      y0 = interpolate(x-mDx, iX, iY);
   }

   if(Global::isValid(y1) && Global::isValid(y0))
      return (y1-y0)/mDx;
   else
      return Global::MV;
}
