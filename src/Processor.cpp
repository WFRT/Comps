#include "Processor.h"
#include "Options.h"

float Processor::mDefaultEfold = 30;
Processor::Processor(const Options& iOptions, const Data& iData) : Component(iOptions), mData(iData) {
   mType = Component::TypeUnspecified;
   if(!iOptions.getValue("efold", mEfold)) {
      mEfold = mDefaultEfold;
   }
}
Processor::~Processor() {}

float Processor::combine(float iOldParameter, float iNewParameter, float iNumNew) const {
   float value = iOldParameter * (mEfold-1)/mEfold + iNewParameter / mEfold * iNumNew;
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

Component::Type Processor::getType() const {
   return mType;
}
