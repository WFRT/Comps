#include "Region.h"
#include "../Poolers/Pooler.h"

SpreaderRegion::SpreaderRegion(const Options& iOptions) : Spreader(iOptions) {

}

bool SpreaderRegion::estimate(const ParameterIo& iParameterIo, Component::Type iType,
         int iDate,
         int iInit,
         float iOffsetCode,
         const Location& iLocation,
         const std::string iVariable,
         int iIndex,
         const Configuration& iConfiguration,
         Parameters& iParameters) const {
   const Pooler* pooler = iParameterIo.getPooler();
   int poolId = pooler->find(iLocation);
   return iParameterIo.read(iType, iDate, iInit, iOffsetCode, poolId, iVariable, iConfiguration, iIndex, iParameters);
}
