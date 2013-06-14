#ifndef PARAMETER_IO_MEMORY_H
#define PARAMETER_IO_MEMORY_H
#include "ParameterIo.h"

class ParameterIoMemory : public ParameterIo {
   public:
      ParameterIoMemory(const Options& iOptions, const Data& iData);
   protected:
      void writeCore() {};
      bool readCore(const Key::Par& iKey, Parameters& iParameters) const {return false;};
};
#endif

