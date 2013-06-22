#include "Fraction.h"
#include "../Data.h"
#include "../Ensemble.h"
#include "../Variables/Variable.h"

MeasureFraction::MeasureFraction(const Options& iOptions, const Data& iData) :
      Measure(iOptions, iData),
      mX(Global::MV),
      mUseMin(false),
      mUseMax(false) {
   iOptions.getValue("x", mX);
   iOptions.getValue("useMin", mUseMin);
   iOptions.getValue("useMax", mUseMax);
   if(!Global::isValid(mX) + !mUseMin + !mUseMax != 1) {
      std::stringstream ss;
      ss << "At least (and at most) one of 'x', 'useMin', and 'useMax', must be specified";
      Global::logger->write(ss.str(), Logger::error);
   }
}

float MeasureFraction::measureCore(const Ensemble& iEnsemble, const Parameters& iParameters) const {
   int counter = 0;
   int total   = 0;

   // Determine where on the variable-axis the discrete point is
   float point = mX;
   if(mUseMin)
      point = Variable::get(iEnsemble.getVariable())->getMin();
   else if(mUseMax)
      point = Variable::get(iEnsemble.getVariable())->getMax();
   assert(Global::isValid(point));

   for(int i = 0; i < iEnsemble.size(); i++) {
      if(Global::isValid(iEnsemble[i])) {
         if(iEnsemble[i] == point) {
            counter++;
         }
         total++;
      }
   }
   float frac = Global::MV;
   if(total > 0)
      frac = (float) counter / total;
   return frac;
}
