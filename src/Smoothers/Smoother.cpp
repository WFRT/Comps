#include "SchemesHeader.inc"

Smoother::Smoother(const Options& iOptions, const Data& iData) : Processor(iOptions, iData) {
   mType = Processor::TypeSmoother;

}
#include "Schemes.inc"
