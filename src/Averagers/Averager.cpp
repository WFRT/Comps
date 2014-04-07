#include "Averager.h"
#include "SchemesHeader.inc"

Averager::Averager(const Options& iOptions, const Data& iData) : Processor(iOptions, iData) {
   mType = Component::TypeAverager;
}
#include "Schemes.inc"
