#include "Processor.h"
#include "Options.h"

float Processor::mDefaultEfold = 30;
Processor::Processor(const Options& iOptions, const Data& iData) : Component(iOptions), mData(iData) {
   mType = Processor::TypeUnspecified;
   if(!iOptions.getValue("efold", mEfold)) {
      mEfold = mDefaultEfold;
   }
}
Processor::~Processor() {}

float Processor::combine(float iOldParameter, float iNewParameter, float iNumNew) const {
   // TODO: How to we weight by number of data point when we don't know how many points have been
   // used previously?
   float value = iOldParameter * (mEfold-1)/mEfold + iNewParameter / mEfold;
   return value;
}

int Processor::numParameters() const {
   if(needsTraining()) {
      return numParametersCore();
   }
   else {
      return 0;
   }
}
int Processor::numParametersCore() const {
   return Global::MV;
}

Processor::Type Processor::getType() const {
   return mType;
}

std::string Processor::getProcessorName(Processor::Type iType) {
   switch(iType) {
      case TypeSelector:
         return "selector";
         break;
      case TypeCorrector:
         return "corrector";
         break;
      case TypeUncertainty:
         return "uncertainty";
         break;
      case TypeContinuous:
         return "continuous";
         break;
      case TypeDiscrete:
         return "discrete";
         break;
      case TypeDiscreteLower:
         return "discreteLower";
         break;
      case TypeDiscreteUpper:
         return "discreteUpper";
         break;
      case TypeCalibrator:
         return "calibrator";
         break;
      case TypeAverager:
         return "averager";
         break;
      case TypeUpdater:
         return "updater";
         break;
      case TypeSmoother:
         return "smoother";
         break;
      case TypeEstimator:
         return "estimator";
         break;
      default:
         return "none";
         break;
   }
}

