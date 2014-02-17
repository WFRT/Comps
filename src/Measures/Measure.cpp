#include "SchemesHeader.inc"
#include "Measure.h"
#include "../Ensemble.h"

Measure::Measure(const Options& iOptions, const Data& iData) :
      Component(iOptions),
      mData(iData),
      mLastMeasure(Global::MV) {
   std::vector<std::string> postTags;
   std::vector<std::string> preTags;
   //! Apply this transofmration to the ensemble members
   iOptions.getValues("preTransforms", preTags);
   //! Apply this transformation to the measure
   iOptions.getValues("postTransforms", postTags);
   for(int i = 0; i < preTags.size(); i++) {
      Transform* transform = Transform::getScheme(preTags[i]);
      mPreTransforms.push_back(transform);
   }
   for(int i = 0; i < postTags.size(); i++) {
      Transform* transform = Transform::getScheme(postTags[i]);
      mPostTransforms.push_back(transform);
   }
}
Measure::~Measure() {
   for(int i = 0; i < mPreTransforms.size(); i++)
      delete mPreTransforms[i];
   for(int i = 0; i < mPostTransforms.size(); i++)
      delete mPostTransforms[i];
}

#include "Schemes.inc"

float Measure::measure(const Ensemble& iEnsemble) const {
   if(iEnsemble.getValues() == mLastEnsemble) {
      return mLastMeasure;
   }
   float value;
   if(mPreTransforms.size() > 0) {
      // Transform the ensemble. This is in a separate if statement so that 
      // a copy of the ensemble isn't made when no transformations are needed
      Ensemble ens = iEnsemble;
      for(int i = 0; i < mPreTransforms.size(); i++) 
         mPreTransforms[i]->transform(ens);
      value = measureCore(ens);
   }
   else {
      value = measureCore(iEnsemble);
   }

   // Transform the measure
   for(int i = 0; i < mPostTransforms.size(); i++) 
      value = mPostTransforms[i]->transform(value);

   // Cache the most recent calculation
   mLastMeasure = value;
   mLastEnsemble = iEnsemble.getValues();
   return value;
}
