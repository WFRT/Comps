#include "Measure.h"
#include "../Data.h"
#include "../Obs.h"
#include "../Parameters.h"
#include "../Ensemble.h"
#include "../Measures/Measure.h"
CorrectorMeasure::CorrectorMeasure(const Options& iOptions, const Data& iData) :
      Corrector(iOptions, iData) {
   std::string measureTag;
   //! Which measure should be used to collapse ensemble?
   iOptions.getValue("measure", measureTag);
   mMeasure = Measure::getScheme(measureTag, mData);
}
CorrectorMeasure::~CorrectorMeasure() {
   delete mMeasure;
}
void CorrectorMeasure::correctCore(const Parameters& iParameters,
      Ensemble& iUnCorrected) const {
   float value = mMeasure->measure(iUnCorrected);

   std::vector<float> values;
   values.push_back(value);
   iUnCorrected.setValues(values);
}
