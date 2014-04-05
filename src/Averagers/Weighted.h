#ifndef AVERAGER_WEIGHTED_H
#define AVERAGER_WEIGHTED_H
#include "../Global.h"
#include "../Options.h"

//! Weighted average based on the skill of the ensemble member
class AveragerWeighted : public Averager {
   public:
      AveragerWeighted(const Options& iOptions, const Data& iData);
      float average(const Ensemble& iValues, const Parameters& iParameters) const;
      void updateParameters(const std::vector<Ensemble>& iValues,
            const std::vector<Obs>& iObs,
            Parameters& iParameters) const;
      void getDefaultParameters(Parameters& iParameters) const;
      bool needsTraining() const;
   private:
      //! Ensemble size is unknown at instantiation, therefore parameters may need to be resized
      void resize(Parameters& iParameters, int iSize) const;
      //! Since members can have 0 errors (due to sampling error), add a small offset, to avoid
      //! division by 0 when computing weights
      static float errorOffset;
      bool mUseSelectorSkill;
      int numParametersCore() const;
};
#endif
