#include "Default.h"
#include "../Member.h"

VariableDefault::VariableDefault(const Options& iOptions, const Data& iData) : Variable(iOptions, iData) {
   iOptions.getRequiredValue("tag", mName);
   iOptions.check();
}

float VariableDefault::computeCore(int iDate,
      int iInit,
      float iOffset,
      const Location& iLocation,
      const Member& iMember,
      Input::Type iType) const {
   // TODO
   return Global::MV;

   std::stringstream ss;
   ss << "Variable: '" << mName << "' is not available in " << Input::getTypeDescription(iType) << " datasets and does not have a custom definition";
   Global::logger->write(ss.str(), Logger::error);
}

std::string VariableDefault::getBaseVariable() const {
   return mName;
}
