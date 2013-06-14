#ifndef SMOOTHER_H
#define SMOOTHER_H
#include "../Global.h"
#include "../Options.h"
#include "../Component.h"
#include "../Ensemble.h"
#include "../Parameters.h"
#include "../Obs.h"

class Smoother: public Component {
   public:
      Smoother(const Options& iOptions, const Data& iData);
      virtual void smooth(const std::vector<float>& iValues,
            const Parameters& iParameters,
            std::vector<float>& iSmoothedValues) const = 0;
      static Smoother* getScheme(const Options& iOptions, const Data& iData);
      static Smoother* getScheme(const std::string& iTag, const Data& iData);
      virtual void updateParameters(const std::vector<float>& iValues,
            const std::vector<Obs>& iObs,
            Parameters& iParameters) const {}
};
#endif
