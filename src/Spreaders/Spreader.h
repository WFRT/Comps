#ifndef SPREADER_H
#define SPREADER_H
#include "../Global.h"
#include "../Component.h"
#include "../Parameters.h"
#include "../Options.h"
#include "../ParameterIos/ParameterIo.h"
#include "../Location.h"
#include "../Configurations/Configuration.h"

class Spreader : public Component {
   public:
      Spreader(const Options& iOptions);
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
      static Spreader* getScheme(const Options& iOptions);
      static Spreader* getScheme(const std::string& iTag);
};
#endif

