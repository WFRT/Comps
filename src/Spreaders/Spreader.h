#ifndef SPREADER_H
#define SPREADER_H
#include "../Global.h"
#include "../Component.h"
#include "../Parameters.h"
#include "../Options.h"
#include "../ParameterIos/ParameterIo.h"
#include "../Location.h"
#include "../Configurations/Configuration.h"
#include "../Data.h"

//! Represents how parameters are spread spatially
class Spreader : public Component {
   public:
      Spreader(const Options& iOptions, const Data& iData);
      virtual bool estimate(const ParameterIo& iParameterIo,
         const Pooler& iPooler,
         Component::Type iType,
         int iDate,
         int iInit,
         float iOffsetCode,
         const Location& iLocation,
         const std::string iVariable,
         int iIndex,
         Parameters& iParameters) const = 0;
      static Spreader* getScheme(const Options& iOptions, const Data& iData);
      static Spreader* getScheme(const std::string& iTag, const Data& iData);
   protected:
      const Data& mData;
};
#endif

