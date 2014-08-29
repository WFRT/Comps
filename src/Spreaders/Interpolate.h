#ifndef SPREADER_INTERPOLATE_H
#define SPREADER_INTERPOLATE_H
#include "Spreader.h"

//! Spread parameters based on inverse distance
class SpreaderInterpolate : public Spreader {
   public:
      SpreaderInterpolate(const Options& iOptions, const Data& iData);
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
   private:
      int mNum;
};
#endif
