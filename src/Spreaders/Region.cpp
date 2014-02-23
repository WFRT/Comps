#include "Region.h"
#include "../Poolers/Pooler.h"

SpreaderRegion::SpreaderRegion(const Options& iOptions, const Data& iData) : Spreader(iOptions, iData) {

}

bool SpreaderRegion::estimate(const ParameterIo& iParameterIo,
         const Pooler& iPooler,
         Component::Type iType,
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
