#include "Fixed.h"

DiscreteFixed::DiscreteFixed(const Options& iOptions, const Data& iData) : Discrete(iOptions, iData) {
   //! Value of the fixed probability
   iOptions.getRequiredValue("value", mValue);
}
