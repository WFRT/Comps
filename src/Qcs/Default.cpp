#include "Default.h"
#include "../Options.h"
#include "../Variables/Variable.h"
#include "../Value.h"

QcDefault::QcDefault(const Options& iOptions, const Data& iData) : Qc(iOptions, iData) {}

bool QcDefault::checkCore(const Value& iValue) const {
   const Variable* var = Variable::get(iValue.getVariable());
   float value = iValue.getValue();

   if((Global::isValid(var->getMin()) && value < var->getMin()) ||
      (Global::isValid(var->getMax()) && value > var->getMax())) {
      std::stringstream ss;
      ss << var->getName() << " value of " << value << " on " << iValue.getDate() << " " << iValue.getOffset() << " assumed missing";
      Global::logger->write(ss.str(), Logger::warning);
      return false;
   }
   return true;
}
