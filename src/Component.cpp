#include "Component.h"
#include "Options.h"

Component::Component(const Options& iOptions, const Data& iData) : mData(iData) {
   if(!iOptions.getValue("efold", mEfold)) {
      mEfold = mDefaultEfold;
   }
   iOptions.getRequiredValue("tag", mTag);
}
Component::~Component() {}

void Component::doesNotExist(const std::string& iClassName) {
   std::stringstream ss;
   ss << "Scheme " << iClassName << " does not exist" << std::endl;
   Global::logger->write(ss.str(), Logger::error);
}

std::string Component::getComponentName(Component::Type iType) {
   switch(iType) {
      case TypeInput:
         return "input";
         break;
      case TypeSelector:
         return "selector";
         break;
      case TypeDownscaler:
         return "downscaler";
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
      case TypeMetric:
         return "metric";
         break;
      case TypeEstimator:
         return "estimator";
         break;
      case TypeDetMetric:
         return "detMetric";
         break;
      default:
         return "none";
         break;
   }
}

float Component::combine(float iOldParameter, float iNewParameter, float iNumNew) const {
   float value = iOldParameter * (mEfold-1)/mEfold + iNewParameter / mEfold * iNumNew;
   return value;
}

void Component::underDevelopment() const {
   std::stringstream ss;
   ss << typeid(*this).name() << " scheme is under development and cannot be selected" << std::endl;
   Global::logger->write(ss.str(), Logger::error);
}

std::string Component::getSchemeName() const {
   std::string name = typeid(*this).name();
   std::stringstream ss;

   // Remove the length digits at the beginning of the component name
   int stringLength = name.length();
   int iStart = 1;
   if(stringLength >= 12)
      iStart = 2;
   else if(stringLength >= 102)
      iStart = 3;
   else
      iStart = 4;

   for(int i = iStart; i < stringLength; i++) {
      ss << name[i];
   }
   return ss.str();
}

std::string Component::getTag() const {
   return mTag;
}
