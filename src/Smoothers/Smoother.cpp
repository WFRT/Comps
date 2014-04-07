#include "SchemesHeader.inc"

Smoother::Smoother(const Options& iOptions, const Data& iData) : Processor(iOptions, iData) {
   mType = Component::TypeSmoother;

}
#include "Schemes.inc"
