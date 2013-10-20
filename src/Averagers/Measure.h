#ifndef AVERAGER_MEASURE_H
#define AVERAGER_MEASURE_H
#include "../Global.h"
#include "../Options.h"
#include "../Measures/Measure.h"

class AveragerMeasure : public Averager {
   public:
      AveragerMeasure(const Options& iOptions, const Data& iData);
      ~AveragerMeasure();
      float average(const Ensemble& iValues, const Parameters& iParameters) const;
      void getDefaultParameters(Parameters& iParameters) const {};
      bool needsTraining() const {return false;};
   private:
      Measure* mMeasure;
};
#endif
