#include "Bypass.h"
#include "../Member.h"

VariableBypass::VariableBypass(std::string iName) : Variable(iName) {}

float VariableBypass::computeCore(const Data& iData,
      int iDate,
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
