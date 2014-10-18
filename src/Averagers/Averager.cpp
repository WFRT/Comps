#include "Averager.h"
#include "SchemesHeader.inc"

Averager::Averager(const Options& iOptions, const Data& iData) : Processor(iOptions, iData) {
   mType = Processor::TypeAverager;
}
#include "Schemes.inc"
