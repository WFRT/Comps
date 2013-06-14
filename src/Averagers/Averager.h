#ifndef AVERAGER_H
#define AVERAGER_H
#include "../Global.h"
#include "../Options.h"
#include "../Component.h"
#include "../Ensemble.h"
#include "../Parameters.h"
#include "../Obs.h"

class Averager: public Component {
   public:
      Averager(const Options& iOptions, const Data& iData);
      virtual float average(const Ensemble& iValues, const Parameters& iParameters) const = 0;
      static Averager* getScheme(const Options& iOptions, const Data& iData);
      static Averager* getScheme(const std::string& iTag, const Data& iData);
      //! Defaults to not updating parameters
      virtual void updateParameters(const Ensemble& iValues,
            const std::vector<Obs>& iObs,
            Parameters& iParameters) const {};
};
#endif
