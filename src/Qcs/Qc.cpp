#include "SchemesHeader.inc"
#include "Qc.h"
#include "../Ensemble.h"
#include "../Value.h"

Qc::Qc(const Options& iOptions, const Data& iData) : Component(iOptions, iData) {}

#include "Schemes.inc"
bool Qc::check(const Value& iValue) const {
   if(!Global::isValid(iValue.value)) {
      return true;
   }
   else {
      return checkCore(iValue);
   }
}

bool Qc::checkCore(const Value& iValue) const {
   return true;
}

bool Qc::qc(Ensemble& iEnsemble) const {
   bool anyChanges = false;
   for(int i = 0; i < iEnsemble.size(); i++) {
      if(!check(Value(iEnsemble[i], iEnsemble.getDate(), iEnsemble.getOffset(), iEnsemble.getLocation(), iEnsemble.getVariable()))) {
         anyChanges = true;
         iEnsemble[i] = Global::MV;
      }
   }
   return anyChanges;
}
