#ifndef CORRECTOR_ANALOG_H
#define CORRECTOR_ANALOG_H
#include "Corrector.h"

class Selector;
class Neighbourhood;

//! Adjust forecast based on bias on analogous days
class CorrectorAnalog : public Corrector {
   public:
      CorrectorAnalog(const Options& iOptions, const Data& iData);
   private:
      void correctCore(const Parameters& iParameters, Ensemble& iUnCorrected) const;
      Selector* mSelector;
      Neighbourhood* mNeighbourhood;
      Selector* mAnalogSelector;
      bool mMultiplicative;
};
#endif
