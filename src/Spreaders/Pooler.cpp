#include "Pooler.h"
#include "../Poolers/Pooler.h"

SpreaderPooler::SpreaderPooler(const Options& iOptions, const Data& iData) : Spreader(iOptions, iData) {

   iOptions.check();
}

bool SpreaderPooler::estimate(const ParameterIo& iParameterIo,
         const Pooler& iPooler,
         Processor::Type iType,
         int iDate,
         int iInit,
         float iOffsetCode,
         const Location& iLocation,
         const std::string iVariable,
         int iIndex,
         Parameters& iParameters) const {
   int poolId = iPooler.find(iLocation);
   return iParameterIo.read(iType, iDate, iInit, iOffsetCode, poolId, iVariable, iIndex, iParameters);
}
