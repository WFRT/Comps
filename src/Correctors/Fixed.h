#ifndef CORRECTOR_FIXED_H
#define CORRECTOR_FIXED_H
#include "Corrector.h"
#include <boost/random.hpp>
#include <boost/random/normal_distribution.hpp>

class CorrectorFixed : public Corrector {
   public:
      CorrectorFixed(const Options& iOptions, const Data& iData);
   protected:
      void correctCore(const Parameters& iParameters, Ensemble& iUnCorrected) const;
      void  getDefaultParametersCore(Parameters& iParameters) const;
      void  updateParametersCore(const std::vector<Ensemble>& iUnCorrected,
            const std::vector<Obs>& iObs,
            Parameters& iParameters) const;
   private:
      mutable boost::variate_generator<boost::mt19937, boost::normal_distribution<> > mRand;
      float mValue;
};
#endif
