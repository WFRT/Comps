#ifndef SPREADER_CRESSMAN_H
#define SPREADER_CRESSMAN_H
#include "Spreader.h"

//! Spread parameters based on inverse distance
class SpreaderCressman : public Spreader {
   public:
      SpreaderCressman(const Options& iOptions, const Data& iData);
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
      float mMaxDistance;
      float mEfoldDistance;
};
#endif
