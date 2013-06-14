#ifndef SMOOTHER_TRIANGLE_H
#define SMOOTHER_TRIANGLE_H
#include "../Global.h"
#include "../Options.h"
#include "../Component.h"
#include "../Ensemble.h"
#include "../Parameters.h"
#include "../Obs.h"
#include "Smoother.h"

class SmootherTriangle: public Smoother {
   public:
      SmootherTriangle(const Options& iOptions, const Data& iData);
      void smooth(const std::vector<float>& iValues,
            const Parameters& iParameters,
            std::vector<float>& iSmoothedValues) const;
      void getDefaultParameters(Parameters& iParameters) const {};
      bool needsTraining() const {return false;};
   private:
      int mWidth;
};
#endif
