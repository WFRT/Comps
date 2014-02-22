#ifndef SPREADER_REGION_H
#define SPREADER_REGION_H
#include "Spreader.h"

class SpreaderRegion : public Spreader {
   public:
      SpreaderRegion(const Options& iOptions);
      bool estimate(const ParameterIo& iParameterIo,
         const Pooler& iPooler,
         Component::Type iType,
         int iDate,
         int iInit,
         float iOffsetCode,
         const Location& iLocation,
         const std::string iVariable,
         int iIndex,
         Parameters& iParameters) const;
};
#endif
