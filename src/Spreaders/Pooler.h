#ifndef SPREADER_REGION_H
#define SPREADER_REGION_H
#include "Spreader.h"

//! Assign parameters based on which parameter pool the location is in
class SpreaderPooler : public Spreader {
   public:
      SpreaderPooler(const Options& iOptions, const Data& iData);
      bool estimate(const ParameterIo& iParameterIo,
         const Pooler& iPooler,
         Processor::Type iType,
         int iDate,
         int iInit,
         float iOffsetCode,
         const Location& iLocation,
         const std::string iVariable,
         int iIndex,
         Parameters& iParameters) const;
};
#endif
