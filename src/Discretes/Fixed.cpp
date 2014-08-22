#include "Fixed.h"

DiscreteFixed::DiscreteFixed(const Options& iOptions, const Data& iData) : Discrete(iOptions, iData) {
   //! Value of the fixed probability
   iOptions.getRequiredValue("value", mValue);
   if(mValue < 0 || mValue > 1) {
      std::stringstream ss;
      ss << "DiscreteFixed: probability mass 'value', must be between 0 and 1";
      Global::logger->write(ss.str(), Logger::error);
   }
   iOptions.check();
}
