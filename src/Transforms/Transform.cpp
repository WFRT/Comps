#include "Transform.h"

Transform::Transform(const Options& iOptions, const Data& iData) : Component(iOptions, iData) {

}
float Transform::transform(float iValue) const {
   if(Global::isValid(iValue))
      return transformCore(iValue);
   else
      return Global::MV;
}

float Transform::inverse(float iValue) const {
   if(Global::isValid(iValue))
      return reverseCore(iValue);
   else
      return Global::MV;
}

void Transform::transform(Ensemble& iEnsemble) const {
   for(int i = 0; i < iEnsemble.size(); i++) {
      iEnsemble[i] = transform(iEnsemble[i]);
   }
}

float Transform::inverse(Ensemble& iEnsemble) const {
   for(int i = 0; i < iEnsemble.size(); i++) {
      iEnsemble[i] = inverse(iEnsemble[i]);
   }
}

float Transform::inverseCore(float iValue) const {
   std::stringstream ss;
   ss << "Transform function '" << Component::getSchemeName() << "' does not have an inverse" << std::endl;;
   Global::logger->write(ss.str(), Logger::error);
}
